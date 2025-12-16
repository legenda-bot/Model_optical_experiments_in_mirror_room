#ifndef WALLDIALOG_H
#define WALLDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "wall.h"

class WallDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WallDialog(Wall* wall, int wallIndex, QWidget *parent = nullptr);

signals:
    void wallConfigurationChanged();

protected:
    void accept() override;

private slots:
    void onMirrorTypeChanged(int index);
    void applyChanges();
    void updateWallPreview();
    void updateRadiusConstraints();

private:
    Wall* m_wall;
    int m_wallIndex;

    // UI elements
    QComboBox* m_mirrorTypeCombo;
    QComboBox* m_sphericalTypeCombo;
    QDoubleSpinBox* m_radiusSpinBox;
    QLabel* m_previewLabel;

    void setupUI();
    void updatePreviewText();
};

#endif // WALLDIALOG_H
