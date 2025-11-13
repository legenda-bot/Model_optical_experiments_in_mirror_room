<<<<<<< HEAD
#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_mirrorRoom(nullptr)
    , m_wallDialog(nullptr)
    , m_currentWallIndex(-1)
{
    setupUI();
    setWindowTitle("Mirror Room Optical Experiments");
    setMinimumSize(800, 600);

    statusBar()->showMessage("Ready to create mirror room");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Create central widget and main layout
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Create mirror room
    m_mirrorRoom = new MirrorRoom(this);
    mainLayout->addWidget(m_mirrorRoom, 1);

    // Create control panel
    QWidget *controlPanel = new QWidget(this);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
    controlPanel->setMaximumWidth(300);

    // Add control groups
    controlLayout->addWidget(createRoomCreationGroup());
    controlLayout->addWidget(createExperimentGroup());
    controlLayout->addWidget(createFileOperationsGroup());
    controlLayout->addStretch();

    mainLayout->addWidget(controlPanel);

    setCentralWidget(centralWidget);

    // Connect signals
    connect(m_mirrorRoom, &MirrorRoom::wallSelected, this, &MainWindow::onWallSelected);
}

void MainWindow::setupRoomCreationGroup()
{
    // This will be implemented in the UI creation methods
}

void MainWindow::setupExperimentGroup()
{
    // This will be implemented in the UI creation methods
}

void MainWindow::setupFileOperationsGroup()
{
    // This will be implemented in the UI creation methods
}

QGroupBox* MainWindow::createRoomCreationGroup()
{
    QGroupBox *groupBox = new QGroupBox("Room Creation");
    QVBoxLayout *layout = new QVBoxLayout(groupBox);

    // Room creation mode
    QHBoxLayout *modeLayout = new QHBoxLayout();
    modeLayout->addWidget(new QLabel("Creation Mode:"));

    m_roomCreationCombo = new QComboBox();
    m_roomCreationCombo->addItem("Draw by Click");
    m_roomCreationCombo->addItem("Regular Polygon");
    modeLayout->addWidget(m_roomCreationCombo);
    layout->addLayout(modeLayout);

    // Walls count for regular polygon
    QHBoxLayout *wallsLayout = new QHBoxLayout();
    wallsLayout->addWidget(new QLabel("Number of Walls:"));

    m_wallsCountSpin = new QSpinBox();
    m_wallsCountSpin->setRange(4, 9);
    m_wallsCountSpin->setValue(4);
    wallsLayout->addWidget(m_wallsCountSpin);
    layout->addLayout(wallsLayout);

    // Clear room button
    m_clearRoomBtn = new QPushButton("Clear Room");
    layout->addWidget(m_clearRoomBtn);

    // Connect signals
    connect(m_roomCreationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onRoomCreationModeChanged);
    connect(m_wallsCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onWallsCountChanged);
    connect(m_clearRoomBtn, &QPushButton::clicked, this, &MainWindow::onClearRoomClicked);

    return groupBox;
}

QGroupBox* MainWindow::createExperimentGroup()
{
    QGroupBox *groupBox = new QGroupBox("Light Ray Experiment");
    QVBoxLayout *layout = new QVBoxLayout(groupBox);

    // Select point button
    QPushButton *selectPointBtn = new QPushButton("Select Start Point on Wall");
    layout->addWidget(selectPointBtn);

    // Angle selection
    QHBoxLayout *angleLayout = new QHBoxLayout();
    angleLayout->addWidget(new QLabel("Angle:"));

    m_angleSpin = new QDoubleSpinBox();
    m_angleSpin->setRange(0, 360);
    m_angleSpin->setValue(45);
    m_angleSpin->setSuffix("°");
    m_angleSpin->setSingleStep(5);
    angleLayout->addWidget(m_angleSpin);

    QPushButton *selectAngleBtn = new QPushButton("Select Angle Visually");
    angleLayout->addWidget(selectAngleBtn);
    layout->addLayout(angleLayout);

    // Start experiment button
    m_startExperimentBtn = new QPushButton("Start Ray Tracing");
    layout->addWidget(m_startExperimentBtn);

    // Connect signals
    connect(m_startExperimentBtn, &QPushButton::clicked, this, &MainWindow::onStartExperimentClicked);
    connect(selectPointBtn, &QPushButton::clicked, this, &MainWindow::onSelectPointClicked);
    connect(selectAngleBtn, &QPushButton::clicked, this, &MainWindow::onSelectAngleClicked);
    connect(m_angleSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onAngleChanged);

    return groupBox;
}


QGroupBox* MainWindow::createFileOperationsGroup()
{
    QGroupBox *groupBox = new QGroupBox("File Operations");
    QVBoxLayout *layout = new QVBoxLayout(groupBox);

    m_saveExperimentBtn = new QPushButton("Save Experiment");
    m_loadExperimentBtn = new QPushButton("Load Experiment");

    layout->addWidget(m_saveExperimentBtn);
    layout->addWidget(m_loadExperimentBtn);

    // Connect signals
    connect(m_saveExperimentBtn, &QPushButton::clicked, this, &MainWindow::onSaveExperimentClicked);
    connect(m_loadExperimentBtn, &QPushButton::clicked, this, &MainWindow::onLoadExperimentClicked);

    return groupBox;
}

void MainWindow::onRoomCreationModeChanged(int index)
{
    m_mirrorRoom->setRoomCreationMode(static_cast<MirrorRoom::RoomCreationMode>(index));
    if (index == 0) {
        statusBar()->showMessage("Click to create room vertices (minimum 4 points)");
    } else {
        statusBar()->showMessage("Regular polygon created");
    }
}

void MainWindow::onWallsCountChanged(int count)
{
    m_mirrorRoom->setNumberOfWalls(count);
    statusBar()->showMessage(QString("Regular polygon with %1 walls created").arg(count));
}

void MainWindow::onStartExperimentClicked()
{
    if (m_mirrorRoom->getRayStartPoint().isNull()) {
        QMessageBox::warning(this, "Experiment Setup",
                             "Please select a starting point on a wall first.");
        return;
    }

    double angle = m_angleSpin->value();
    m_mirrorRoom->startRayExperiment(angle);
    statusBar()->showMessage(QString("Ray tracing started from selected point at %1°").arg(angle));
}

void MainWindow::onWallSelected(int wallIndex)
{
    m_currentWallIndex = wallIndex;

    // Get the wall from mirror room
    Wall* wall = m_mirrorRoom->getWall(wallIndex);

    if (!wall) {
        statusBar()->showMessage("Error: Selected wall not found");
        return;
    }

    // Close existing dialog if open
    if (m_wallDialog) {
        m_wallDialog->close();
        delete m_wallDialog;
        m_wallDialog = nullptr;
    }

    // Create and show wall configuration dialog
    m_wallDialog = new WallDialog(wall, this);
    connect(m_wallDialog, &WallDialog::wallConfigurationChanged,
            this, &MainWindow::onWallConfigurationChanged);

    m_wallDialog->show();

    statusBar()->showMessage(QString("Wall %1 selected for configuration").arg(wallIndex + 1));
}

void MainWindow::onWallConfigurationChanged()
{
    m_mirrorRoom->update(); // Force redraw
    statusBar()->showMessage("Wall configuration updated");
}

void MainWindow::onSaveExperimentClicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Experiment", "", "Mirror Room Files (*.mrf)");
    if (!filename.isEmpty()) {
        m_mirrorRoom->saveExperiment(filename);
        statusBar()->showMessage("Experiment saved to: " + filename);
    }
}

void MainWindow::onLoadExperimentClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load Experiment", "", "Mirror Room Files (*.mrf)");
    if (!filename.isEmpty()) {
        m_mirrorRoom->loadExperiment(filename);
        statusBar()->showMessage("Experiment loaded from: " + filename);
    }
}

void MainWindow::onClearRoomClicked()
{
    m_mirrorRoom->clearRoom();
    statusBar()->showMessage("Room cleared");
}

void MainWindow::onSelectPointClicked()
{
    if (!m_mirrorRoom->getWalls().isEmpty()) {
        m_mirrorRoom->setSelectingStartPoint(true);
        statusBar()->showMessage("Click on ANY part of a wall to select ray start point");
    } else {
        QMessageBox::warning(this, "Room Setup",
                             "Please create a room with walls first.");
    }
}

void MainWindow::onSelectAngleClicked()
{
    if (!m_mirrorRoom->getRayStartPoint().isNull()) {
        m_mirrorRoom->setSelectingAngle(true);
        statusBar()->showMessage("Click to set ray direction - current angle will be shown");
    } else {
        QMessageBox::warning(this, "Experiment Setup",
                             "Please select a starting point first.");
    }
}

void MainWindow::onAngleChanged(double angle)
{
    m_mirrorRoom->setCurrentAngle(angle);
}
=======
#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_mirrorRoom(nullptr)
    , m_wallDialog(nullptr)
    , m_currentWallIndex(-1)
{
    setupUI();
    setWindowTitle("Mirror Room Optical Experiments");
    setMinimumSize(800, 600);

    statusBar()->showMessage("Ready to create mirror room");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Create central widget and main layout
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Create mirror room
    m_mirrorRoom = new MirrorRoom(this);
    mainLayout->addWidget(m_mirrorRoom, 1);

    // Create control panel
    QWidget *controlPanel = new QWidget(this);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
    controlPanel->setMaximumWidth(300);

    // Add control groups
    controlLayout->addWidget(createRoomCreationGroup());
    controlLayout->addWidget(createExperimentGroup());
    controlLayout->addWidget(createFileOperationsGroup());
    controlLayout->addStretch();

    mainLayout->addWidget(controlPanel);

    setCentralWidget(centralWidget);

    // Connect signals
    connect(m_mirrorRoom, &MirrorRoom::wallSelected, this, &MainWindow::onWallSelected);
}

void MainWindow::setupRoomCreationGroup()
{
    // This will be implemented in the UI creation methods
}

void MainWindow::setupExperimentGroup()
{
    // This will be implemented in the UI creation methods
}

void MainWindow::setupFileOperationsGroup()
{
    // This will be implemented in the UI creation methods
}

QGroupBox* MainWindow::createRoomCreationGroup()
{
    QGroupBox *groupBox = new QGroupBox("Room Creation");
    QVBoxLayout *layout = new QVBoxLayout(groupBox);

    // Room creation mode
    QHBoxLayout *modeLayout = new QHBoxLayout();
    modeLayout->addWidget(new QLabel("Creation Mode:"));

    m_roomCreationCombo = new QComboBox();
    m_roomCreationCombo->addItem("Draw by Click");
    m_roomCreationCombo->addItem("Regular Polygon");
    modeLayout->addWidget(m_roomCreationCombo);
    layout->addLayout(modeLayout);

    // Walls count for regular polygon
    QHBoxLayout *wallsLayout = new QHBoxLayout();
    wallsLayout->addWidget(new QLabel("Number of Walls:"));

    m_wallsCountSpin = new QSpinBox();
    m_wallsCountSpin->setRange(4, 9);
    m_wallsCountSpin->setValue(4);
    wallsLayout->addWidget(m_wallsCountSpin);
    layout->addLayout(wallsLayout);

    // Clear room button
    m_clearRoomBtn = new QPushButton("Clear Room");
    layout->addWidget(m_clearRoomBtn);

    // Connect signals
    connect(m_roomCreationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onRoomCreationModeChanged);
    connect(m_wallsCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onWallsCountChanged);
    connect(m_clearRoomBtn, &QPushButton::clicked, this, &MainWindow::onClearRoomClicked);

    return groupBox;
}

QGroupBox* MainWindow::createExperimentGroup()
{
    QGroupBox *groupBox = new QGroupBox("Light Ray Experiment");
    QVBoxLayout *layout = new QVBoxLayout(groupBox);

    // Start position
    QHBoxLayout *posLayout = new QHBoxLayout();
    posLayout->addWidget(new QLabel("Start Position:"));

    m_startXSpin = new QDoubleSpinBox();
    m_startXSpin->setRange(0, 1000);
    m_startXSpin->setValue(300);
    m_startXSpin->setSuffix(" px");

    m_startYSpin = new QDoubleSpinBox();
    m_startYSpin->setRange(0, 1000);
    m_startYSpin->setValue(250);
    m_startYSpin->setSuffix(" px");

    posLayout->addWidget(new QLabel("X:"));
    posLayout->addWidget(m_startXSpin);
    posLayout->addWidget(new QLabel("Y:"));
    posLayout->addWidget(m_startYSpin);
    layout->addLayout(posLayout);

    // Angle
    QHBoxLayout *angleLayout = new QHBoxLayout();
    angleLayout->addWidget(new QLabel("Start Angle:"));

    m_angleSpin = new QDoubleSpinBox();
    m_angleSpin->setRange(0, 180);
    m_angleSpin->setValue(45);
    m_angleSpin->setSuffix("°");
    angleLayout->addWidget(m_angleSpin);
    layout->addLayout(angleLayout);

    // Start experiment button
    m_startExperimentBtn = new QPushButton("Start Experiment");
    layout->addWidget(m_startExperimentBtn);

    // Connect signals
    connect(m_startExperimentBtn, &QPushButton::clicked, this, &MainWindow::onStartExperimentClicked);

    return groupBox;
}

QGroupBox* MainWindow::createFileOperationsGroup()
{
    QGroupBox *groupBox = new QGroupBox("File Operations");
    QVBoxLayout *layout = new QVBoxLayout(groupBox);

    m_saveExperimentBtn = new QPushButton("Save Experiment");
    m_loadExperimentBtn = new QPushButton("Load Experiment");

    layout->addWidget(m_saveExperimentBtn);
    layout->addWidget(m_loadExperimentBtn);

    // Connect signals
    connect(m_saveExperimentBtn, &QPushButton::clicked, this, &MainWindow::onSaveExperimentClicked);
    connect(m_loadExperimentBtn, &QPushButton::clicked, this, &MainWindow::onLoadExperimentClicked);

    return groupBox;
}

void MainWindow::onRoomCreationModeChanged(int index)
{
    m_mirrorRoom->setRoomCreationMode(static_cast<MirrorRoom::RoomCreationMode>(index));
    statusBar()->showMessage(index == 0 ? "Click to create room vertices" : "Regular polygon created");
}

void MainWindow::onWallsCountChanged(int count)
{
    m_mirrorRoom->setNumberOfWalls(count);
    statusBar()->showMessage(QString("Regular polygon with %1 walls created").arg(count));
}

void MainWindow::onStartExperimentClicked()
{
    QPointF startPoint(m_startXSpin->value(), m_startYSpin->value());
    double angle = m_angleSpin->value();

    m_mirrorRoom->startRayExperiment(startPoint, angle);
    statusBar()->showMessage(QString("Experiment started from (%1, %2) at %3°").arg(startPoint.x()).arg(startPoint.y()).arg(angle));
}

void MainWindow::onWallSelected(int wallIndex)
{
    m_currentWallIndex = wallIndex;

    // Get the wall from mirror room
    Wall* wall = m_mirrorRoom->getWall(wallIndex);

    if (!wall) {
        statusBar()->showMessage("Error: Selected wall not found");
        return;
    }

    // Close existing dialog if open
    if (m_wallDialog) {
        m_wallDialog->close();
        delete m_wallDialog;
        m_wallDialog = nullptr;
    }

    // Create and show wall configuration dialog
    m_wallDialog = new WallDialog(wall, this);
    connect(m_wallDialog, &WallDialog::wallConfigurationChanged,
            this, &MainWindow::onWallConfigurationChanged);

    m_wallDialog->show();

    statusBar()->showMessage(QString("Wall %1 selected for configuration - %2")
                                 .arg(wallIndex + 1)
                                 .arg(wall->toString()));
}

void MainWindow::onWallConfigurationChanged()
{
    m_mirrorRoom->update(); // Force redraw
    statusBar()->showMessage("Wall configuration updated - room redrawn");
}

void MainWindow::onSaveExperimentClicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Experiment", "", "Mirror Room Files (*.mrf)");
    if (!filename.isEmpty()) {
        m_mirrorRoom->saveExperiment(filename);
        statusBar()->showMessage("Experiment saved to: " + filename);
    }
}

void MainWindow::onLoadExperimentClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load Experiment", "", "Mirror Room Files (*.mrf)");
    if (!filename.isEmpty()) {
        m_mirrorRoom->loadExperiment(filename);
        statusBar()->showMessage("Experiment loaded from: " + filename);
    }
}

void MainWindow::onClearRoomClicked()
{
    m_mirrorRoom->clearRoom();
    statusBar()->showMessage("Room cleared");
}
>>>>>>> dbcaaaab8187a40d6d56fbc2b69f275e8cfb3978
