// Main entry point for QtGPT
#include <QApplication>
#include "QtGPT.h"
#include "ChatWidget.h"
#include "PipeHandler.h"
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Initialize QtGPT application singleton
    QtGPT *qtgpt = new QtGPT();
    
    // Initialize main window
    ChatWidget *chatWidget = new ChatWidget();
    qtgpt->setMainWindow(chatWidget);
    
    // Initialize pipe handler
    if (qtgpt->pipeHandler()->openPipe()) {
        qtgpt->pipeHandler()->startEventLoop();
    }
    
    // Show main window
    chatWidget->show();
    
    // Load plugins
    qtgpt->loadPlugins();
    
    // Initialize LLM context
    qtgpt->initContext();
    
    return app.exec();
}
