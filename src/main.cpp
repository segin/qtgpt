// Main entry point for QtGPT
#include <QApplication>
#include <QObject>
#include "QtGPT.h"
#include "ChatWidget.h"
#include "PipeHandler.h"
#include "MenuBar.h"
#include "SettingsDialog.h"
#include "ChatHistoryViewer.h"
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QMetaObject>
#include <QTextEdit>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Initialize QtGPT application singleton
    QtGPT *qtgpt = new QtGPT(nullptr);
    
    // Initialize main window
    ChatWidget *chatWidget = new ChatWidget();
    qtgpt->setMainWindow(chatWidget);
    
    // Initialize menu bar
    MenuBar *menuBar = new MenuBar();
    chatWidget->setMenuBar(menuBar);
    
    // Initialize pipe handler
    if (qtgpt->pipeHandler()->openPipe()) {
        qtgpt->pipeHandler()->startEventLoop();
    }
    
    // Show main window
    chatWidget->resize(600, 500);
    chatWidget->show();
    
    // Load plugins
    qtgpt->loadPlugins();
    
    // Initialize LLM context
    qtgpt->initContext();
    
    // Initialize settings dialog
    SettingsDialog *settingsDialog = new SettingsDialog(chatWidget);
    qtgpt->setSettingsDialog(settingsDialog);
    
    // Connect MenuBar signals to QtGPT slots
    QObject::connect(menuBar, &MenuBar::openChatRequested, qtgpt, &QtGPT::openConversationSlot);
    QObject::connect(menuBar, &MenuBar::saveChatRequested, qtgpt, &QtGPT::saveConversationSlot);
    QObject::connect(menuBar, &MenuBar::settingsRequested, qtgpt, &QtGPT::openSettingsSlot);
    QObject::connect(menuBar, &MenuBar::clearChatRequested, qtgpt, &QtGPT::clearChatSlot);
    QObject::connect(menuBar, &MenuBar::quitRequested, qtgpt, &QtGPT::quitSlot);
    QObject::connect(menuBar, &MenuBar::cutRequested, chatWidget, [chatWidget]() {
        if (QWidget *focused = chatWidget->focusWidget()) {
            if (QTextEdit *edit = qobject_cast<QTextEdit*>(focused)) {
                edit->cut();
            }
        }
    });
    QObject::connect(menuBar, &MenuBar::copyRequested, chatWidget, [chatWidget]() {
        if (QWidget *focused = chatWidget->focusWidget()) {
            if (QTextEdit *edit = qobject_cast<QTextEdit*>(focused)) {
                edit->copy();
            }
        }
    });
    QObject::connect(menuBar, &MenuBar::pasteRequested, chatWidget, [chatWidget]() {
        if (QWidget *focused = chatWidget->focusWidget()) {
            if (QTextEdit *edit = qobject_cast<QTextEdit*>(focused)) {
                edit->paste();
            }
        }
    });
    QObject::connect(menuBar, &MenuBar::selectAllRequested, chatWidget, [chatWidget]() {
        if (QWidget *focused = chatWidget->focusWidget()) {
            if (QTextEdit *edit = qobject_cast<QTextEdit*>(focused)) {
                edit->selectAll();
            }
        }
    });
    
    // Connect SettingsDialog signals to QtGPT methods
    QObject::connect(settingsDialog, &SettingsDialog::SettingsApplied, qtgpt, &QtGPT::loadSettingsSlot);
    QObject::connect(settingsDialog, &SettingsDialog::accepted, qtgpt, &QtGPT::loadSettingsSlot);
    QObject::connect(settingsDialog, &SettingsDialog::rejected, qtgpt, &QtGPT::discardChangesSlot);
    QObject::connect(settingsDialog, &SettingsDialog::ProviderChanged, qtgpt, &QtGPT::settingsChanged);
    QObject::connect(settingsDialog, &SettingsDialog::TabChanged, qtgpt, &QtGPT::settingsChanged);
    QObject::connect(qtgpt, &QtGPT::GetModels, settingsDialog, &SettingsDialog::onGeminiLoadModels);
    QObject::connect(qtgpt, &QtGPT::UseSelectedModel, settingsDialog, &SettingsDialog::onGeminiUseModel);
    
    // Connect QtGPT signals to ChatWidget methods
    QObject::connect(qtgpt, &QtGPT::chatMessageAdded, chatWidget, [qtgpt, chatWidget]() {
        chatWidget->renderHistory(QVector<QMap<QString, QString>>::fromList(qtgpt->chatHistory()));
    });
    QObject::connect(qtgpt, &QtGPT::clearChatSignal, chatWidget, &ChatWidget::clearMessages);
    
    // Connect ChatWidget signals to QtGPT slots
    QObject::connect(chatWidget, &ChatWidget::sendMessageSignal, qtgpt, [qtgpt](const QString &text) {
        qtgpt->addChatMessage("user", text);
    });
    QObject::connect(chatWidget, &ChatWidget::retryRequested, qtgpt, &QtGPT::retryRequestedSlot);
    QObject::connect(chatWidget, &ChatWidget::editRequested, qtgpt, &QtGPT::editRequestedSlot);
    QObject::connect(chatWidget, &ChatWidget::clearChat, qtgpt, &QtGPT::clearChatSlot);
    
    // Load saved settings
    qtgpt->loadSettingsSlot();
    
    return app.exec();
}
