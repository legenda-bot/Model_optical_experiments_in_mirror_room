// MainWindow: Qt UI wiring for room setup and ray tracing controls.

#include "mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStatusBar>
#include <QSlider>
#include <cmath>

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
    connect(m_mirrorRoom, &MirrorRoom::angleUpdated, m_angleSpin, &QDoubleSpinBox::setValue);
    connect(m_mirrorRoom, &MirrorRoom::simulationStateChanged, this, &MainWindow::onSimulationStateChanged);
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

void MainWindow::onSimulationStateChanged(bool running)
{
    m_simulationRunning = running;
    bool enabled = !running;
    m_roomCreationCombo->setEnabled(enabled);
    m_wallsCountSpin->setEnabled(enabled);
    m_polygonSizeSlider->setEnabled(enabled);
    m_startExperimentBtn->setEnabled(enabled);
    m_selectPointBtn->setEnabled(enabled);
    m_selectAngleBtn->setEnabled(enabled);
    m_angleSpin->setEnabled(enabled);
    m_speedSlider->setEnabled(enabled);
    // Clear buttons remain enabled so user can stop/reset
    if (m_wallDialog) {
        m_wallDialog->setEditingEnabled(enabled);
        if (running) {
            statusBar()->showMessage("Simulation running: wall editing is disabled");
        }
    }
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

    // Polygon size slider (for regular polygon)
    QHBoxLayout *sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(new QLabel("Polygon size:"));
    m_polygonSizeSlider = new QSlider(Qt::Horizontal);
    m_polygonSizeSlider->setRange(20, 75); // percent of widget size, ограничение сверху для стабильной отрисовки
    m_polygonSizeSlider->setValue(50);
    sizeLayout->addWidget(m_polygonSizeSlider);
    layout->addLayout(sizeLayout);

    // Clear room button
    m_clearRoomBtn = new QPushButton("Clear Room");
    layout->addWidget(m_clearRoomBtn);

    // Connect signals
    connect(m_roomCreationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onRoomCreationModeChanged);
    connect(m_wallsCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onWallsCountChanged);
    connect(m_polygonSizeSlider, &QSlider::valueChanged,
            m_mirrorRoom, &MirrorRoom::setPolygonScale);
    connect(m_clearRoomBtn, &QPushButton::clicked, this, &MainWindow::onClearRoomClicked);

    return groupBox;
}

QGroupBox* MainWindow::createExperimentGroup()
{
    QGroupBox *groupBox = new QGroupBox("Light Ray Experiment");
    QVBoxLayout *layout = new QVBoxLayout(groupBox);

    // Select point button
    m_selectPointBtn = new QPushButton("Select Start Point (Wall/Arc)");
    layout->addWidget(m_selectPointBtn);

    // Angle selection
    QHBoxLayout *angleLayout = new QHBoxLayout();
    angleLayout->addWidget(new QLabel("Angle:"));

    m_angleSpin = new QDoubleSpinBox();
    m_angleSpin->setRange(0, 360);
    m_angleSpin->setValue(45);
    m_angleSpin->setSuffix(" deg");
    m_angleSpin->setSingleStep(5);
    m_angleSpin->setDecimals(2);
    m_angleSpin->setKeyboardTracking(false);
    m_angleSpin->setWrapping(true);
    angleLayout->addWidget(m_angleSpin);

    m_selectAngleBtn = new QPushButton("Select Angle Visually");
    angleLayout->addWidget(m_selectAngleBtn);
    layout->addLayout(angleLayout);

    // Speed slider
    QHBoxLayout *speedLayout = new QHBoxLayout();
    speedLayout->addWidget(new QLabel("Ray speed:"));
    m_speedSlider = new QSlider(Qt::Horizontal);
    m_speedSlider->setRange(100, 1000); // ms per segment
    // Inverted mapping: higher slider value => faster animation (smaller ms per segment).
    m_speedSlider->setValue(700); // maps to 400ms effective
    speedLayout->addWidget(m_speedSlider);
    layout->addLayout(speedLayout);

    // Start experiment button
    m_startExperimentBtn = new QPushButton("Start Ray Tracing");
    layout->addWidget(m_startExperimentBtn);

    // Clear ray button
    m_clearRayBtn = new QPushButton("Clear Ray");
    layout->addWidget(m_clearRayBtn);

    // Connect signals
    connect(m_startExperimentBtn, &QPushButton::clicked, this, &MainWindow::onStartExperimentClicked);
    connect(m_selectPointBtn, &QPushButton::clicked, this, &MainWindow::onSelectPointClicked);
    connect(m_selectAngleBtn, &QPushButton::clicked, this, &MainWindow::onSelectAngleClicked);
    connect(m_angleSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onAngleChanged);
    connect(m_speedSlider, &QSlider::valueChanged, this, [this](int value) {
        const int min = m_speedSlider->minimum();
        const int max = m_speedSlider->maximum();
        m_mirrorRoom->setAnimationSpeed(min + max - value);
    });
    connect(m_clearRayBtn, &QPushButton::clicked, m_mirrorRoom, &MirrorRoom::clearRay);
    m_mirrorRoom->setAnimationSpeed(m_speedSlider->minimum() + m_speedSlider->maximum() - m_speedSlider->value());

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
    statusBar()->showMessage(QString("Ray tracing started from selected point at %1 deg").arg(angle));
}

void MainWindow::onWallSelected(int wallIndex)
{
    if (m_simulationRunning) {
        statusBar()->showMessage("Simulation running: wall editing is disabled");
        return;
    }
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
    m_wallDialog = new WallDialog(wall, wallIndex, this);
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
    QString filename = QFileDialog::getSaveFileName(this, "Save Experiment", "", "Text Files (*.txt)");
    if (!filename.isEmpty()) {
        if (QFileInfo(filename).suffix().isEmpty()) filename += ".txt";
        m_mirrorRoom->saveExperiment(filename);
        statusBar()->showMessage("Experiment saved to: " + filename);
    }
}

void MainWindow::onLoadExperimentClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load Experiment", "", "Text Files (*.txt)");
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
        statusBar()->showMessage("Click on a wall (or its arc) to select ray start point");
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
    if (!std::isfinite(angle)) {
        QMessageBox::critical(this, "Invalid Angle", "Angle must be a finite number.");
        m_angleSpin->setValue(0.0);
        return;
    }
    // Normalize to [0, 360)
    double normalized = std::fmod(angle, 360.0);
    if (normalized < 0.0) normalized += 360.0;
    if (std::abs(normalized - angle) > 1e-9) {
        m_angleSpin->setValue(normalized);
    }
    m_mirrorRoom->setCurrentAngle(normalized);
}
