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

#include "netcontrol.h"
#include "ui_netcontrol.h"

#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QtDebug>

#include "setting_defines.h"
#include "utils.h"

NETControl::NETControl(QTcpSocket *client, QWidget *parent) : QWidget(parent),
                                                              ui(new Ui::NETControl),
                                                              netToolBar("Net Toolbar"),
                                                              openAction("Open", this)
{
    ui->setupUi(this);

    tcpClient = client;

    // setup actions
    openAction.setCheckable(true);
    // openAction.setShortcut(QKeySequence("F12"));
    openAction.setToolTip("Net");
    QObject::connect(&openAction, &QAction::triggered,
                     this, &NETControl::openActionTriggered);

    netToolBar.addAction(&openAction);

    ui->pbNetOpen->setDefaultAction(&openAction);

    // ui->leNetIP->setInputMask("000.000.000.000; ");  
    ui->leNetPort->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));   

    //connect(tcpclient, &QTcpSocket::readyRead, this, &MainWindow::tcpclientreaddata);
}

NETControl::~NETControl()
{
    delete ui;
}

void NETControl::openActionTriggered(bool checked)
{
    QString ip = ui->leNetIP->text();
    int port = ui->leNetPort->text().toInt();


    qDebug() << "IP:  " << ip;
    // qDebug() << "PORT:" << port;

    if(tcpClient->state() != QAbstractSocket::ConnectedState)
    {
        tcpClient->connectToHost(ip, port);

        if (!tcpClient->waitForConnected(100))
        {
            qDebug() << "Connection failed!";

            openAction.setChecked(false);

            return;
        }

        openAction.setChecked(true);

    }else{
        tcpClient->close();
        openAction.setChecked(false);
    }
}

QToolBar *NETControl::toolBar()
{
    return &netToolBar;
}

void NETControl::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Net);
    settings->setValue(SG_Net_IP, ui->leNetIP->text());
    settings->setValue(SG_Net_Port, ui->leNetPort->text());
    settings->endGroup();
}

void NETControl::loadSettings(QSettings* settings)
{

    settings->beginGroup(SettingGroup_Net);

    QString ip = settings->value(SG_Net_IP, QString()).toString();
    if (!ip.isEmpty())
    {
        ui->leNetIP->setText(ip);
    }

    QString port = settings->value(SG_Net_Port, QString()).toString();
    if (!port.isEmpty())
    {
        ui->leNetPort->setText(port);
    }

    settings->endGroup();
}