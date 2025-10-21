#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "mirrorroom.h"
#include "walldialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onRoomCreationModeChanged(int index);
    void onWallsCountChanged(int count);
    void onStartExperimentClicked();
    void onWallSelected(int wallIndex);
    void onWallConfigurationChanged();
    void onSaveExperimentClicked();
    void onLoadExperimentClicked();
    void onClearRoomClicked();

private:
    void setupUI();
    void setupRoomCreationGroup();
    void setupExperimentGroup();
    void setupFileOperationsGroup();

    // Добавляем объявления методов создания групп
    QGroupBox* createRoomCreationGroup();
    QGroupBox* createExperimentGroup();
    QGroupBox* createFileOperationsGroup();

    MirrorRoom *m_mirrorRoom;
    WallDialog *m_wallDialog;

    // UI elements
    QComboBox *m_roomCreationCombo;
    QSpinBox *m_wallsCountSpin;
    QDoubleSpinBox *m_startXSpin;
    QDoubleSpinBox *m_startYSpin;
    QDoubleSpinBox *m_angleSpin;
    QPushButton *m_startExperimentBtn;
    QPushButton *m_saveExperimentBtn;
    QPushButton *m_loadExperimentBtn;
    QPushButton *m_clearRoomBtn;

    int m_currentWallIndex;
};

#endif // MAINWINDOW_H
