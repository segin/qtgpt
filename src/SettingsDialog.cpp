// SettingsDialog implementation (stub)
#include "SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::showSettings()
{
    this->show();
}

void SettingsDialog::hideSettings()
{
    this->hide();
}

bool SettingsDialog::isShown() const
{
    return this->isVisible();
}

bool SettingsDialog::applySettings()
{
    return true;
}

void SettingsDialog::cancelSettings()
{
    this->reject();
}

void SettingsDialog::loadModelList(dp_provider_type_t provider, const QString &api_key)
{
    // Stub implementation
}

void SettingsDialog::clearModelList()
{
    // Stub implementation
}

// Private slot implementations (stubs)
void SettingsDialog::onApply() {}
void SettingsDialog::onCancel() {}
void SettingsDialog::onProviderChanged(int index) {}
void SettingsDialog::onTabChanged(int index) {}
void SettingsDialog::onGeminiModelChanged() {}
void SettingsDialog::onOpenAIModelChanged() {}
void SettingsDialog::onAnthropicModelChanged() {}
void SettingsDialog::onLoadModels() {}
void SettingsDialog::onUseModelSelected() {}
void SettingsDialog::onModelListError(const QString &error) {}
void SettingsDialog::onHistoryLimitsToggled(bool checked) {}
void SettingsDialog::onEnterSendsToggled(bool checked) {}
void SettingsDialog::onAppendPromptToggled(bool checked) {}
