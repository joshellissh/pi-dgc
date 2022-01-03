#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include "hwdialog.h"
#include "ui_hwdialog.h"

HWDialog::HWDialog(VehicleValues &vehicle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HWDialog)
{
    ui->setupUi(this);

    this->vehicle = &vehicle;

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &HWDialog::updateValues);
    timer->start(100); // ~10 FPS
}

HWDialog::~HWDialog()
{
    delete ui;
}

void HWDialog::updateValues()
{
    if (isVisible() && vehicle->initialized) {
        ui->plainTextEdit->setPlainText(vehicle->serialize());
        ui->ppmCurrent->setText(QString::number(vehicle->getPPM()));
        if (ui->ppmSpinBox->value() != vehicle->getPPM()) {
            QPalette pal = ui->ppmSpinBox->palette();
            pal.setColor(ui->ppmSpinBox->backgroundRole(), Qt::red);
            ui->ppmSpinBox->setPalette(pal);
        } else {
            QPalette pal = ui->ppmSpinBox->palette();
            pal.setColor(ui->ppmSpinBox->backgroundRole(), Qt::gray);
            ui->ppmSpinBox->setPalette(pal);
        }

        QDateTime date = QDateTime::currentDateTime();
        QString formattedTime = date.toString("ddd MMMM dd yyyy hh:mm:ss AP");
        QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
        ui->currentDateTime->setText(formattedTimeMsg);
    }

    update();
}

void HWDialog::showEvent(QShowEvent *event) {
    ui->ppmSpinBox->setValue(vehicle->getPPM());
    ui->ppmSpinBox->setStyleSheet("QSpinBox { background-color: white; }");

    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());
}

void HWDialog::accept()
{
    vehicle->newPPM = ui->ppmSpinBox->value();
    vehicle->writePPM = true;

    QString datePart = ui->dateEdit->date().toString("\"yyyy-MM-dd ");
    QString timePart = ui->timeEdit->time().toString("hh:mm\"");
    QString dateTimeString ("sudo date -s ");
    dateTimeString.append(datePart).append(timePart);

    int systemDateTimeStatus = system(dateTimeString.toStdString().c_str());
    if (systemDateTimeStatus == -1) {
        qDebug() << "Failed to change date time";
    }

    int systemHwClockStatus = system("sudo hwclock -w");
    if (systemHwClockStatus == -1 ) {
        qDebug() << "Failed to sync hardware clock";
    }

    QMessageBox msgBox;
    msgBox.setText("Values have been saved to EEPROM.");
    msgBox.exec();
}

void HWDialog::reject()
{
    this->done(0);
}
