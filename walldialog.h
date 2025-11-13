<<<<<<< HEAD
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
    explicit WallDialog(Wall* wall, QWidget *parent = nullptr);

signals:
    void wallConfigurationChanged();

private slots:
    void onMirrorTypeChanged(int index);
    void applyChanges();
    void updateWallPreview();

private:
    Wall* m_wall;

    // UI elements
    QComboBox* m_mirrorTypeCombo;
    QComboBox* m_sphericalTypeCombo;
    QDoubleSpinBox* m_radiusSpinBox;
    QLabel* m_previewLabel;

    void setupUI();
    void updatePreviewText();
};

#endif // WALLDIALOG_H
=======
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
    explicit WallDialog(Wall* wall, QWidget *parent = nullptr);

signals:
    void wallConfigurationChanged();

private slots:
    void onMirrorTypeChanged(int index);
    void applyChanges();
    void updateWallPreview();

private:
    Wall* m_wall;

    // UI elements
    QComboBox* m_mirrorTypeCombo;
    QComboBox* m_sphericalTypeCombo;
    QDoubleSpinBox* m_radiusSpinBox;
    QLabel* m_previewLabel;

    void setupUI();
    void updatePreviewText();
};

#endif // WALLDIALOG_H
>>>>>>> dbcaaaab8187a40d6d56fbc2b69f275e8cfb3978
