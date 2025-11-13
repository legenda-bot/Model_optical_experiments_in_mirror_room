<<<<<<< HEAD
#include "walldialog.h"
#include <QGroupBox>
#include <QFormLayout>

WallDialog::WallDialog(Wall* wall, QWidget *parent)
    : QDialog(parent)
    , m_wall(wall)
{
    setupUI();
    setWindowTitle(QString("Wall Configuration - Wall %1").arg(reinterpret_cast<quintptr>(wall) % 1000)); // Unique title
    setMinimumSize(400, 300);
}

void WallDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Configuration group
    QGroupBox* configGroup = new QGroupBox("Wall Configuration");
    QFormLayout* formLayout = new QFormLayout(configGroup);

    // Mirror type selection
    m_mirrorTypeCombo = new QComboBox();
    m_mirrorTypeCombo->addItem("Flat Mirror", Wall::Flat);
    m_mirrorTypeCombo->addItem("Spherical Mirror", Wall::Spherical);
    formLayout->addRow("Mirror Type:", m_mirrorTypeCombo);

    // Spherical type selection
    m_sphericalTypeCombo = new QComboBox();
    m_sphericalTypeCombo->addItem("Concave", Wall::Concave);
    m_sphericalTypeCombo->addItem("Convex", Wall::Convex);
    formLayout->addRow("Spherical Type:", m_sphericalTypeCombo);

    // Radius selection
    m_radiusSpinBox = new QDoubleSpinBox();
    m_radiusSpinBox->setRange(50.0, 1000.0);
    m_radiusSpinBox->setValue(100.0);
    m_radiusSpinBox->setSuffix(" px");
    m_radiusSpinBox->setSingleStep(10.0);
    formLayout->addRow("Radius:", m_radiusSpinBox);

    mainLayout->addWidget(configGroup);

    // Preview group
    QGroupBox* previewGroup = new QGroupBox("Current Settings");
    QVBoxLayout* previewLayout = new QVBoxLayout(previewGroup);

    m_previewLabel = new QLabel();
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 10px; border: 1px solid #ccc; }");
    previewLayout->addWidget(m_previewLabel);

    mainLayout->addWidget(previewGroup);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* applyButton = new QPushButton("Apply");
    QPushButton* okButton = new QPushButton("OK");
    QPushButton* cancelButton = new QPushButton("Cancel");

    applyButton->setDefault(true);

    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Set initial values from wall
    m_mirrorTypeCombo->setCurrentIndex(m_wall->mirrorType());
    m_sphericalTypeCombo->setCurrentIndex(m_wall->sphericalType());
    m_radiusSpinBox->setValue(m_wall->radius());

    // Connect signals
    connect(m_mirrorTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WallDialog::onMirrorTypeChanged);
    connect(m_mirrorTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WallDialog::updateWallPreview);
    connect(m_sphericalTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WallDialog::updateWallPreview);
    connect(m_radiusSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &WallDialog::updateWallPreview);

    connect(applyButton, &QPushButton::clicked, this, &WallDialog::applyChanges);
    connect(okButton, &QPushButton::clicked, this, [this]() {
        applyChanges();
        accept();
    });
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Initial update
    onMirrorTypeChanged(m_wall->mirrorType());
    updateWallPreview();
}

void WallDialog::onMirrorTypeChanged(int index)
{
    bool isSpherical = (index == Wall::Spherical);
    m_sphericalTypeCombo->setEnabled(isSpherical);
    m_radiusSpinBox->setEnabled(isSpherical);
}

void WallDialog::applyChanges()
{
    if (!m_wall) return;

    // Save current settings to wall
    m_wall->setMirrorType(static_cast<Wall::MirrorType>(m_mirrorTypeCombo->currentIndex()));
    m_wall->setSphericalType(static_cast<Wall::SphericalType>(m_sphericalTypeCombo->currentIndex()));
    m_wall->setRadius(m_radiusSpinBox->value());

    emit wallConfigurationChanged();
    updateWallPreview();
}

void WallDialog::updateWallPreview()
{
    updatePreviewText();
}

void WallDialog::updatePreviewText()
{
    QString mirrorType = m_mirrorTypeCombo->currentText();
    QString sphericalType = m_sphericalTypeCombo->isEnabled() ? m_sphericalTypeCombo->currentText() : "N/A";
    QString radius = m_radiusSpinBox->isEnabled() ? QString::number(m_radiusSpinBox->value()) + " px" : "N/A";

    QString previewText = QString(
                              "<b>Current Configuration:</b><br>"
                              "Mirror Type: %1<br>"
                              "Spherical Type: %2<br>"
                              "Radius: %3"
                              ).arg(mirrorType, sphericalType, radius);

    m_previewLabel->setText(previewText);
}
=======
#include "walldialog.h"
#include <QGroupBox>
#include <QFormLayout>

WallDialog::WallDialog(Wall* wall, QWidget *parent)
    : QDialog(parent)
    , m_wall(wall)
{
    setupUI();
    setWindowTitle(QString("Wall Configuration - Wall %1").arg(reinterpret_cast<quintptr>(wall) % 1000)); // Unique title
    setMinimumSize(400, 300);
}

void WallDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Configuration group
    QGroupBox* configGroup = new QGroupBox("Wall Configuration");
    QFormLayout* formLayout = new QFormLayout(configGroup);

    // Mirror type selection
    m_mirrorTypeCombo = new QComboBox();
    m_mirrorTypeCombo->addItem("Flat Mirror", Wall::Flat);
    m_mirrorTypeCombo->addItem("Spherical Mirror", Wall::Spherical);
    formLayout->addRow("Mirror Type:", m_mirrorTypeCombo);

    // Spherical type selection
    m_sphericalTypeCombo = new QComboBox();
    m_sphericalTypeCombo->addItem("Concave", Wall::Concave);
    m_sphericalTypeCombo->addItem("Convex", Wall::Convex);
    formLayout->addRow("Spherical Type:", m_sphericalTypeCombo);

    // Radius selection
    m_radiusSpinBox = new QDoubleSpinBox();
    m_radiusSpinBox->setRange(50.0, 1000.0);
    m_radiusSpinBox->setValue(100.0);
    m_radiusSpinBox->setSuffix(" px");
    m_radiusSpinBox->setSingleStep(10.0);
    formLayout->addRow("Radius:", m_radiusSpinBox);

    mainLayout->addWidget(configGroup);

    // Preview group
    QGroupBox* previewGroup = new QGroupBox("Current Settings");
    QVBoxLayout* previewLayout = new QVBoxLayout(previewGroup);

    m_previewLabel = new QLabel();
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 10px; border: 1px solid #ccc; }");
    previewLayout->addWidget(m_previewLabel);

    mainLayout->addWidget(previewGroup);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* applyButton = new QPushButton("Apply");
    QPushButton* okButton = new QPushButton("OK");
    QPushButton* cancelButton = new QPushButton("Cancel");

    applyButton->setDefault(true);

    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Set initial values from wall
    m_mirrorTypeCombo->setCurrentIndex(m_wall->mirrorType());
    m_sphericalTypeCombo->setCurrentIndex(m_wall->sphericalType());
    m_radiusSpinBox->setValue(m_wall->radius());

    // Connect signals
    connect(m_mirrorTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WallDialog::onMirrorTypeChanged);
    connect(m_mirrorTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WallDialog::updateWallPreview);
    connect(m_sphericalTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WallDialog::updateWallPreview);
    connect(m_radiusSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &WallDialog::updateWallPreview);

    connect(applyButton, &QPushButton::clicked, this, &WallDialog::applyChanges);
    connect(okButton, &QPushButton::clicked, this, [this]() {
        applyChanges();
        accept();
    });
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Initial update
    onMirrorTypeChanged(m_wall->mirrorType());
    updateWallPreview();
}

void WallDialog::onMirrorTypeChanged(int index)
{
    bool isSpherical = (index == Wall::Spherical);
    m_sphericalTypeCombo->setEnabled(isSpherical);
    m_radiusSpinBox->setEnabled(isSpherical);
}

void WallDialog::applyChanges()
{
    if (!m_wall) return;

    // Save current settings to wall
    m_wall->setMirrorType(static_cast<Wall::MirrorType>(m_mirrorTypeCombo->currentIndex()));
    m_wall->setSphericalType(static_cast<Wall::SphericalType>(m_sphericalTypeCombo->currentIndex()));
    m_wall->setRadius(m_radiusSpinBox->value());

    emit wallConfigurationChanged();
    updateWallPreview();
}

void WallDialog::updateWallPreview()
{
    updatePreviewText();
}

void WallDialog::updatePreviewText()
{
    QString mirrorType = m_mirrorTypeCombo->currentText();
    QString sphericalType = m_sphericalTypeCombo->isEnabled() ? m_sphericalTypeCombo->currentText() : "N/A";
    QString radius = m_radiusSpinBox->isEnabled() ? QString::number(m_radiusSpinBox->value()) + " px" : "N/A";

    QString previewText = QString(
                              "<b>Current Configuration:</b><br>"
                              "Mirror Type: %1<br>"
                              "Spherical Type: %2<br>"
                              "Radius: %3"
                              ).arg(mirrorType, sphericalType, radius);

    m_previewLabel->setText(previewText);
}
>>>>>>> dbcaaaab8187a40d6d56fbc2b69f275e8cfb3978
