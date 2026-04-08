#ifndef MENUBAR_H
#define MENUBAR_H

#include <QMenuBar>
#include <QMenu>
#include <QAction>

class MenuBar : public QMenuBar
{
    Q_OBJECT

public:
    explicit MenuBar(QWidget *parent = nullptr);

signals:
    void openChatRequested();
    void saveChatRequested();
    void settingsRequested();
    void clearChatRequested();
    void quitRequested();
    void cutRequested();
    void copyRequested();
    void pasteRequested();
    void selectAllRequested();

private slots:
    void onOpenChat();
    void onSaveChat();
    void onSettings();
    void onClearChat();
    void onQuit();
    void onCut();
    void onCopy();
    void onPaste();
    void onSelectAll();

private:
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_settingsAction;
    QAction *m_clearChatAction;
    QAction *m_quitAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_selectAllAction;
};

#endif // MENUBAR_H
