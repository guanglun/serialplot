/*
  Copyright © 2021 Hasan Yavuz Özderya

  This file is part of serialplot.

  serialplot is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  serialplot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dataformatpanel.h"
#include "ui_dataformatpanel.h"

#include <QRadioButton>
#include <QtDebug>

#include "utils.h"
#include "setting_defines.h"

DataFormatPanel::DataFormatPanel(QIODevice* serial,QIODevice* tcp, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataFormatPanel),
    bsReaderSerial(serial, this),
    asciiReaderSerial(serial, this),
    framedReaderSerial(serial, this),
    demoReaderSerial(serial, this),
    bsReaderNet(tcp, this),
    asciiReaderNet(tcp, this),
    framedReaderNet(tcp, this),
    demoReaderNet(tcp, this)
{
    ui->setupUi(this);

    selectDeviceType = 0;
    paused = false;
    readerBeforeDemo = nullptr;
    _bytesRead = 0;

    // initalize default reader
    currentReader = &bsReaderSerial;
    bsReaderSerial.enable();
    ui->rbBinary->setChecked(true);
    ui->horizontalLayout->addWidget(bsReaderSerial.settingsWidget(), 1);

    // initalize reader selection buttons
    readerSelectButtons.addButton(ui->rbBinary);
    readerSelectButtons.addButton(ui->rbAscii);
    readerSelectButtons.addButton(ui->rbFramed);

    connect(ui->rbBinary, &QRadioButton::toggled, [this](bool checked)
            {
                if (checked) 
                {
                    if(selectDeviceType == 0)
                        selectReader(&bsReaderSerial);
                    else
                        selectReader(&bsReaderNet);
                }
            });

    connect(ui->rbAscii, &QRadioButton::toggled, [this](bool checked)
            {
                if (checked)
                {
                    if(selectDeviceType == 0)
                        selectReader(&asciiReaderSerial);
                    else
                        selectReader(&asciiReaderNet);
                }
            });

    connect(ui->rbFramed, &QRadioButton::toggled, [this](bool checked)
            {
                if (checked) {
                    if(selectDeviceType == 0)
                        selectReader(&framedReaderSerial);
                    else
                        selectReader(&framedReaderNet);
                }
            });
}

DataFormatPanel::~DataFormatPanel()
{
    delete ui;
}

unsigned DataFormatPanel::numChannels() const
{
    return currentReader->numChannels();
}

Source* DataFormatPanel::activeSource()
{
    return currentReader;
}

void DataFormatPanel::pause(bool enabled)
{
    paused = enabled;
    currentReader->pause(enabled);
    demoReaderSerial.pause(enabled);
}

void DataFormatPanel::enableDemo(bool demoEnabled)
{
    if (demoEnabled)
    {
        readerBeforeDemo = currentReader;
        demoReaderSerial.setNumChannels(readerBeforeDemo->numChannels());
        selectReader(&demoReaderSerial);
    }
    else
    {
        Q_ASSERT(readerBeforeDemo != nullptr);
        selectReader(readerBeforeDemo);
    }

    // disable/enable reader selection buttons during/after demo
    ui->rbAscii->setDisabled(demoEnabled);
    ui->rbBinary->setDisabled(demoEnabled);
    ui->rbFramed->setDisabled(demoEnabled);
}

bool DataFormatPanel::isDemoEnabled() const
{
    return currentReader == &demoReaderSerial;
}

void DataFormatPanel::selectReaderDevice(int deviceType)
{
    qDebug() << "selectReaderDevice" << deviceType;
    selectDeviceType = deviceType;

    if(selectDeviceType == 0)
    {
        if(currentReader == &bsReaderNet)
        {
            selectReader(&bsReaderSerial);
        }else if(currentReader == &asciiReaderNet)
        {
            selectReader(&asciiReaderSerial);
        }else if(currentReader == &framedReaderNet)
        {
            selectReader(&framedReaderSerial);
        }
    }else if(selectDeviceType == 1)
    {
        if(currentReader == &bsReaderSerial)
        {
            selectReader(&bsReaderNet);
        }else if(currentReader == &asciiReaderSerial)
        {
            selectReader(&asciiReaderNet);
        }else if(currentReader == &framedReaderSerial)
        {
            selectReader(&framedReaderNet);
        }
    }
}

void DataFormatPanel::selectReader(AbstractReader* reader)
{
    currentReader->enable(false);
    reader->enable();

    // re-connect signals
    disconnect(currentReader, 0, this, 0);

    // switch the settings widget
    ui->horizontalLayout->removeWidget(currentReader->settingsWidget());
    currentReader->settingsWidget()->hide();
    ui->horizontalLayout->addWidget(reader->settingsWidget(), 1);
    reader->settingsWidget()->show();

    reader->pause(paused);

    currentReader = reader;
    emit sourceChanged(currentReader);
}

uint64_t DataFormatPanel::bytesRead()
{
    _bytesRead += currentReader->getBytesRead();
    return _bytesRead;
}

void DataFormatPanel::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_DataFormat);

    // save selected data format (current reader)
    QString format;
    AbstractReader* selectedReader = isDemoEnabled() ? readerBeforeDemo : currentReader;
    if (selectedReader == &bsReaderSerial || selectedReader == &bsReaderNet)
    {
        format = "binary";
    }
    else if (selectedReader == &asciiReaderSerial || selectedReader == &asciiReaderNet)
    {
        format = "ascii";
    }
    else // framed reader
    {
        format = "custom";
    }
    settings->setValue(SG_DataFormat_Format, format);

    settings->endGroup();

    // save reader settings
    bsReaderSerial.saveSettings(settings);
    asciiReaderSerial.saveSettings(settings);
    framedReaderSerial.saveSettings(settings);
}

void DataFormatPanel::loadSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_DataFormat);

    // load selected format
    QString format = settings->value(
        SG_DataFormat_Format, QString()).toString();

    if (format == "binary")
    {
        selectReader(&bsReaderSerial);
        ui->rbBinary->setChecked(true);
    }
    else if (format == "ascii")
    {
        selectReader(&asciiReaderSerial);
        ui->rbAscii->setChecked(true);
    }
    else if (format == "custom")
    {
        selectReader(&framedReaderSerial);
        ui->rbFramed->setChecked(true);
    } // else current selection stays

    settings->endGroup();

    // load reader settings
    bsReaderSerial.loadSettings(settings);
    asciiReaderSerial.loadSettings(settings);
    framedReaderSerial.loadSettings(settings);
}
