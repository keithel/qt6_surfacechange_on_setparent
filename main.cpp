
#include <QApplication>
#include <windows.h>
#include <tchar.h>
#include "QWinHost.h"
#include "eventlurker.h"

#include <QDebug>
#include <QString>
#include <QMainWindow>
#include <QDockWidget>
#include <QMessageBox>
#include <QQuickWindow>
#include <QQuickWidget>
#include <QUrl>


LRESULT CALLBACK WindowProc(_In_ HWND hwnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT: {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT r{};
        GetClientRect(hwnd, &r);
        HBRUSH b = CreateSolidBrush(RGB(100, 100, 150));
        FillRect(hdc, &r, b);
        DeleteObject(b);

        SetTextColor(hdc, 0x00000000);
        LPCWSTR message = L"Native Center View";
        DrawText(
            hdc,
            message, -1,
            &r,
            DT_SINGLELINE | DT_CENTER | DT_VCENTER );
        EndPaint(hwnd, &ps);

        return 0;
    }
    case WM_SIZE:
    {
        InvalidateRect(hwnd, nullptr, FALSE);
        break;
    }
    case WM_DESTROY:
    {
        QString msg = "Warning: Native Center View got destroyed!";
        qWarning().noquote() << msg;

        break;
    }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

HWND createWin32Window(QWinHost *winHost) {
    // Register our embedded test window class
    auto hInstance = GetModuleHandle(NULL);
    WNDCLASS wc;
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 255));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = L"EmbeddedWindow";

    if (!RegisterClass(&wc))
        return nullptr;

    // Create native win32 window that gets embedded in our QMainWindow's central area
    HWND embededWin = CreateWindow(L"EmbeddedWindow", // Predefined class;
                                   L"EmbeddedWindow", // caption
                                   WS_TABSTOP | WS_VISIBLE | WS_CHILD, // Styles
                                   0, // x position
                                   0, // y position
                                   0, // Button width
                                   0, // Button height
                                   (HWND)winHost->winId(), // Parent window
                                   nullptr, // No menu.
                                   hInstance,
                                   nullptr); // Pointer not needed.
    return embededWin;
}

int main( int argc, char *argv[] )
{
    //---------------------------------------------------------------------
    // Set correct Qt dpi scaling environment
    qputenv("QT_SCALE_FACTOR", "1");
    // In Qt6 the HDPI scaling is by default enabled, so we disable it here to match our Qt5 behavior
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
    //---------------------------------------------------------------------

    EventLurker eventLurker;

    QApplication app( argc, argv );

    // Set up the winHost - native win32 window container.
    QWinHost* winHost = new QWinHost();
    winHost->setObjectName("winHost");
    winHost->installEventFilter(&eventLurker);
    HWND embeddedWin = createWin32Window(winHost);
    if (!embeddedWin) {
        delete winHost;
        return -1;
    }
    winHost->setWindow(embeddedWin);

    // Create Top Level MainWindow
    auto mainWindow = new QMainWindow();
    mainWindow->setObjectName("mainWindow");
    mainWindow->installEventFilter(&eventLurker);
    mainWindow->setWindowTitle(QString("TLW Surface Change on Dock - Qt%1.%2.%3").arg(QT_VERSION_MAJOR).arg(QT_VERSION_MINOR).arg(QT_VERSION_PATCH));
    mainWindow->setCentralWidget(winHost);

    mainWindow->resize(800, 600);
    mainWindow->show();

    auto tlwSurfaceType = mainWindow->windowHandle()->surfaceType();
    qDebug() << "MainWindow surface type:" << tlwSurfaceType;

    // Create dock widget for hosting a QML window
    auto dockWidget = new QDockWidget("QML Dock Window", mainWindow);
    dockWidget->setObjectName("dockWidget");
    dockWidget->installEventFilter(&eventLurker);
    dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    mainWindow->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    dockWidget->setFloating(true);

    QObject::connect(dockWidget, &QDockWidget::topLevelChanged, mainWindow, [mainWindow, tlwSurfaceType](){
            auto currentSurfaceType = mainWindow->windowHandle()->surfaceType();
            if (tlwSurfaceType != currentSurfaceType)
            {
                qDebug() << "MainWindow surface type changed to :" << currentSurfaceType;
                QMessageBox::warning(mainWindow, "Warning", "MainWindow surface type changed!");
            }
        });

    // Create OpenGl surface based QML window
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    auto quickWidget = new QQuickWidget(dockWidget);
    quickWidget->setObjectName("quickWidget");
    quickWidget->installEventFilter(&eventLurker);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quickWidget->setSource(QUrl("qrc:///Panel.qml"));
    dockWidget->setWidget(quickWidget);

    //---------------------------------------------------------------------
    // Now manually dock back the floating QML dock widget into our top level main window.
    // Notice that the entire native platform window hierarchy will be destroyed,
    // due to the surface type change from RasterSurface to OpenGLSurface
    //---------------------------------------------------------------------

    int ret = app.exec();
    delete mainWindow;
    return ret;
}
