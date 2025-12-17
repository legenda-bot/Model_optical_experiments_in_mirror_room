// MainWindow: top-level UI class.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSlider>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include "mirrorroom.h"
#include "walldialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectPointClicked();
    void onSelectAngleClicked();
    void onAngleChanged(double angle);
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
    void onSimulationStateChanged(bool running);

    //          
    QGroupBox* createRoomCreationGroup();
    QGroupBox* createExperimentGroup();
    QGroupBox* createFileOperationsGroup();

    MirrorRoom *m_mirrorRoom;
    WallDialog *m_wallDialog;

    // UI elements
    QComboBox *m_roomCreationCombo;
    QSpinBox *m_wallsCountSpin;
    QDoubleSpinBox *m_angleSpin;
    QSlider *m_polygonSizeSlider;
    QSlider *m_speedSlider;
    QPushButton *m_selectPointBtn;
    QPushButton *m_selectAngleBtn;
    QPushButton *m_startExperimentBtn;
    QPushButton *m_clearRayBtn;
    QPushButton *m_saveExperimentBtn;
    QPushButton *m_loadExperimentBtn;
    QPushButton *m_clearRoomBtn;

    int m_currentWallIndex;
    bool m_simulationRunning {false};
};

#endif // MAINWINDOW_H
