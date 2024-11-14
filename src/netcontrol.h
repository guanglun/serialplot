/*
  Copyright © 2019 Hasan Yavuz Özderya

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

#ifndef NETCONTROL_H
#define NETCONTROL_H

#include <QWidget>
#include <QButtonGroup>
#include <QTcpSocket>
#include <QStringList>
#include <QToolBar>
#include <QAction>
#include <QComboBox>
#include <QSettings>
#include <QTimer>

namespace Ui {
class NetControl;
}

class NetControl : public QWidget
{
    Q_OBJECT

public:
    explicit NetControl(QTcpSocket *tcpClient, QWidget* parent = 0);
    ~NetControl();

    QTcpSocket *tcpClient;
    QToolBar* toolBar();

    /// Stores port settings into a `QSettings`
    void saveSettings(QSettings* settings);
    /// Loads port settings from a `QSettings`. If open serial port is closed.
    void loadSettings(QSettings* settings);

private:
    Ui::NetControl *ui;

    QToolBar netToolBar;
    QAction openAction;

private slots:

    void openActionTriggered(bool checked);

signals:
    void selectReaderDevice(int device);
};

#endif // NETCONTROL_H
