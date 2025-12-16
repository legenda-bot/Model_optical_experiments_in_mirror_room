#include "walldialog.h"
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QtGlobal>
#include <cmath>

WallDialog::WallDialog(Wall* wall, int wallIndex, QWidget *parent)
    : QDialog(parent)
    , m_wall(wall)
    , m_wallIndex(wallIndex)
{
    setupUI();
    setWindowTitle(QString("Wall Configuration - Wall %1").arg(m_wallIndex + 1));
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
    // Максимум делаем большим, а "слишком большой" валидируем при Apply/OK с сообщением.
    m_radiusSpinBox->setRange(1e-3, 1.0e9);
    m_radiusSpinBox->setValue(100.0);
    m_radiusSpinBox->setSuffix(" px");
    m_radiusSpinBox->setSingleStep(5.0);
    m_radiusSpinBox->setDecimals(2);
    m_radiusSpinBox->setKeyboardTracking(false);
    m_radiusSpinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
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

    okButton->setDefault(true);

    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Set initial values from wall
    m_mirrorTypeCombo->setCurrentIndex(m_wall->mirrorType());
    m_sphericalTypeCombo->setCurrentIndex(m_wall->sphericalType());
    m_radiusSpinBox->setValue(m_wall->radius());
    updateRadiusConstraints();

    // Connect signals
    connect(m_mirrorTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WallDialog::onMirrorTypeChanged);
    connect(m_mirrorTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WallDialog::updateWallPreview);
    connect(m_mirrorTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WallDialog::updateRadiusConstraints);
    connect(m_sphericalTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WallDialog::updateWallPreview);
    connect(m_sphericalTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &WallDialog::updateRadiusConstraints);
    connect(m_radiusSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &WallDialog::updateWallPreview);

    connect(applyButton, &QPushButton::clicked, this, &WallDialog::applyChanges);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Initial update
    onMirrorTypeChanged(m_wall->mirrorType());
    updateRadiusConstraints();
    updateWallPreview();
}

void WallDialog::onMirrorTypeChanged(int index)
{
    bool isSpherical = (index == Wall::Spherical);
    m_sphericalTypeCombo->setEnabled(isSpherical);
    m_radiusSpinBox->setEnabled(isSpherical);
}

void WallDialog::accept()
{
    // Гарантированно применяем настройки при OK/Enter, даже если accept() вызывается не через кнопку.
    applyChanges();
    QDialog::accept();
}

void WallDialog::applyChanges()
{
    if (!m_wall) return;

    const auto mirrorType = static_cast<Wall::MirrorType>(m_mirrorTypeCombo->currentIndex());
    const auto sphericalType = static_cast<Wall::SphericalType>(m_sphericalTypeCombo->currentIndex());
    const double radius = m_radiusSpinBox->value();

    if (mirrorType != Wall::Flat && mirrorType != Wall::Spherical) {
        QMessageBox::critical(this, "Invalid Input", "Mirror type is invalid.");
        return;
    }
    if (sphericalType != Wall::Concave && sphericalType != Wall::Convex) {
        QMessageBox::critical(this, "Invalid Input", "Spherical type is invalid.");
        return;
    }

    if (mirrorType == Wall::Spherical) {
        if (!std::isfinite(radius)) {
            QMessageBox::critical(this, "Invalid Radius", "Radius must be a finite number.");
            m_radiusSpinBox->setFocus();
            m_radiusSpinBox->selectAll();
            return;
        }

        const double minRadius = qMax(1e-3, m_wall->line().length() * 0.5);
        // Делаем верхнюю границу зависимой от длины стены, иначе для очень длинных стен
        // базовый радиус (L/2) будет всегда считаться "слишком большим".
        const double maxRadius = qMax(1.0e6, minRadius * 10.0);
        // Допуски: 1px по запросу + небольшая относительная поправка на float/округление.
        const double epsAbs = 1.0;
        const double epsRel = 1e-6 * qMax(1.0, minRadius);
        const double eps = epsAbs + epsRel;

        if (radius + eps < minRadius) {
            QMessageBox::critical(this, "Invalid Radius",
                                  QString("Radius is too small for this wall.\n"
                                          "It must be at least %1 px (wall length / 2).")
                                      .arg(minRadius, 0, 'f', 2));
            m_radiusSpinBox->setValue(minRadius);
            return;
        }
        if (radius > maxRadius + eps) {
            QMessageBox::critical(this, "Invalid Radius",
                                  QString("Radius is too large.\n"
                                          "Please use a radius <= %1 px.")
                                      .arg(maxRadius, 0, 'f', 0));
            m_radiusSpinBox->setValue(maxRadius);
            return;
        }
    }

    // Save current settings to wall
    m_wall->setMirrorType(mirrorType);
    m_wall->setSphericalType(sphericalType);
    // Если радиус чуть меньше минимума (в пределах допуска), поднимаем до минимума без ошибки,
    // чтобы окружность всегда проходила через концы стены.
    if (mirrorType == Wall::Spherical) {
        const double minRadius = qMax(1e-3, m_wall->line().length() * 0.5);
        m_wall->setRadius(qMax(radius, minRadius));
    } else {
        m_wall->setRadius(radius);
    }

    emit wallConfigurationChanged();
    updateWallPreview();
}

void WallDialog::updateWallPreview()
{
    updatePreviewText();
}

void WallDialog::updateRadiusConstraints()
{
    if (!m_wall || !m_radiusSpinBox) return;

    const bool isSpherical = (m_mirrorTypeCombo->currentIndex() == Wall::Spherical);
    if (!isSpherical) return;

    // Геометрическое ограничение: окружность должна проходить через концы стены,
    // поэтому R >= длина_стены / 2.
    const double minRadius = qMax(1e-3, m_wall->line().length() * 0.5);
    m_radiusSpinBox->setMinimum(minRadius);
    if (m_radiusSpinBox->value() < minRadius) {
        m_radiusSpinBox->setValue(minRadius);
    }
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
