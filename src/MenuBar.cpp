#include "MenuBar.h"
#include <QIcon>
#include <QStyle>
#include <QApplication>

MenuBar::MenuBar(QWidget *parent)
    : QMenuBar(parent)
{
    m_fileMenu = addMenu(tr("File"));
    m_editMenu = addMenu(tr("Edit"));

    m_openAction = new QAction(tr("Open Conversation..."), this);
    m_openAction->setIcon(QIcon::fromTheme("document-open", QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon)));
    m_openAction->setShortcut(QKeySequence::Open);
    connect(m_openAction, &QAction::triggered, this, &MenuBar::onOpenChat);
    m_fileMenu->addAction(m_openAction);

    m_saveAction = new QAction(tr("Save Conversation As..."), this);
    m_saveAction->setIcon(QIcon::fromTheme("document-save-as", QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    m_saveAction->setShortcut(QKeySequence::Save);
    connect(m_saveAction, &QAction::triggered, this, &MenuBar::onSaveChat);
    m_fileMenu->addAction(m_saveAction);

    m_fileMenu->addSeparator();

    m_settingsAction = new QAction(tr("Settings..."), this);
    m_settingsAction->setIcon(QIcon::fromTheme("preferences-system", QApplication::style()->standardIcon(QStyle::SP_ComputerIcon)));
    m_settingsAction->setShortcut(QKeySequence::Preferences);
    connect(m_settingsAction, &QAction::triggered, this, &MenuBar::onSettings);
    m_fileMenu->addAction(m_settingsAction);

    m_clearChatAction = new QAction(tr("Clear Chat"), this);
    m_clearChatAction->setIcon(QIcon::fromTheme("edit-clear", QApplication::style()->standardIcon(QStyle::SP_DialogDiscardButton)));
    connect(m_clearChatAction, &QAction::triggered, this, &MenuBar::onClearChat);
    m_fileMenu->addAction(m_clearChatAction);

    m_fileMenu->addSeparator();

    m_quitAction = new QAction(tr("Exit"), this);
    m_quitAction->setIcon(QIcon::fromTheme("application-exit", QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton)));
    m_quitAction->setShortcut(QKeySequence::Quit);
    connect(m_quitAction, &QAction::triggered, this, &MenuBar::onQuit);
    m_fileMenu->addAction(m_quitAction);

    m_fileMenu->addSeparator();

    m_cutAction = new QAction(tr("Cut"), this);
    m_cutAction->setIcon(QIcon::fromTheme("edit-cut", QApplication::style()->standardIcon(QStyle::SP_LineEditClearButton)));
    m_cutAction->setShortcut(QKeySequence::Cut);
    connect(m_cutAction, &QAction::triggered, this, &MenuBar::onCut);
    m_editMenu->addAction(m_cutAction);

    m_copyAction = new QAction(tr("Copy"), this);
    m_copyAction->setIcon(QIcon::fromTheme("edit-copy", QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    m_copyAction->setShortcut(QKeySequence::Copy);
    connect(m_copyAction, &QAction::triggered, this, &MenuBar::onCopy);
    m_editMenu->addAction(m_copyAction);

    m_pasteAction = new QAction(tr("Paste"), this);
    m_pasteAction->setIcon(QIcon::fromTheme("edit-paste", QApplication::style()->standardIcon(QStyle::SP_FileDialogListView)));
    m_pasteAction->setShortcut(QKeySequence::Paste);
    connect(m_pasteAction, &QAction::triggered, this, &MenuBar::onPaste);
    m_editMenu->addAction(m_pasteAction);

    m_editMenu->addSeparator();

    m_selectAllAction = new QAction(tr("Select All"), this);
    m_selectAllAction->setIcon(QIcon::fromTheme("edit-select-all", QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView)));
    m_selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(m_selectAllAction, &QAction::triggered, this, &MenuBar::onSelectAll);
    m_editMenu->addAction(m_selectAllAction);
}

void MenuBar::onOpenChat()
{
    emit openChatRequested();
}

void MenuBar::onSaveChat()
{
    emit saveChatRequested();
}

void MenuBar::onSettings()
{
    emit settingsRequested();
}

void MenuBar::onClearChat()
{
    emit clearChatRequested();
}

void MenuBar::onQuit()
{
    emit quitRequested();
}

void MenuBar::onCut()
{
    emit cutRequested();
}

void MenuBar::onCopy()
{
    emit copyRequested();
}

void MenuBar::onPaste()
{
    emit pasteRequested();
}

void MenuBar::onSelectAll()
{
    emit selectAllRequested();
}
