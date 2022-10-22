import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

ApplicationWindow {
    id: applicationWindow
    Material.theme: Material.Dark
    Material.accent: Material.BlueGrey
    Material.primary: Material.BlueGrey
    width: 800
    height: 600
    visible: true
    title: qsTr("LEGO Train")
    property bool disconnect: true

    header: ToolBar {

        contentHeight: toolButtonScan.implicitHeight

        Row{

            spacing: 4

            ToolButton {
                id: toolButtonScan
                text: "\u2630"
                font.pixelSize: Qt.application.font.pixelSize * 1.6

                onClicked: {
                    scanButton.enabled=true;
                    scanButton.text = disconnect ? "Scan" : "Disconnect"
                    drawer.open()
                }
            }


            ToolButton {
                id: toolButtonRun
                text: "\u25b6"
                font.pixelSize: Qt.application.font.pixelSize * 2.0
                //  enabled: false
                checkable: true

                onClicked: {

                    bledevice.startAcquisition(toolButtonRun.checked)

                    if(checked) {


                    }
                }
            }

            ToolButton {
                id: toolButtonSave
                text: "\u2601"
                font.pixelSize: Qt.application.font.pixelSize * 2.0

                onClicked: {

                    bledevice.saveFile();

                }
            }

        }

                Image {
                    id: batteryIcon
                    source: "icons/batt.png"
                    y:6
                    anchors.right: parent.right
                    anchors.rightMargin: 10
               }


    }

    Drawer {
        id: drawer
        width: 250
        height: applicationWindow.height

        Button {
            id: scanButton
            width: parent.width
            text: "Scan"
            onClicked: {

                listView.enabled=false

                if(disconnect) {

                    text="Scanning..."
                    enabled = false
                    busyIndicator.running=true
                    bledevice.startScan()

                } else {

                    bledevice.disconnectFromDevice()
                }
            }
        }

        ListView {
            id: listView
            anchors.fill: parent
            anchors.topMargin: 50
            anchors.bottomMargin: 50
            width: parent.width
            clip: true

            model: bledevice.deviceListModel

            delegate: CheckDelegate {
                id: checkDelegate
                text: (index+1)+".  "+modelData
                width: listView.width

                onCheckedChanged: {
                    console.log("checked", modelData, index)
                    scanButton.enabled=false;
                    scanButton.text="Connecting to "+modelData
                    listView.enabled = false;
                    bledevice.startConnect(index)
                }
            }
        }

        BusyIndicator {
            id: busyIndicator
            Material.accent: "Orange"
            anchors.centerIn: parent
            running: false
        }
    }

    Rectangle
    {
        id: background
        anchors.fill: parent
        color: "#484848"

        Row{
            spacing: 10

            Frame{
                id: frame1
                width:background.width/4
                height: background.height

                Label {
                    id: labelBatt1
                    anchors.top: parent.top
                    width: parent.width
                    text: "Batt: ---%"
                    horizontalAlignment: "AlignHCenter"
                }
                Label {
                    id: labelPower1
                    anchors.top: labelBatt1.bottom
                    width: parent.width
                    text: "Power: ---%"
                    color: "red"
                    horizontalAlignment: "AlignHCenter"
                }

                Slider{
                    id: slider1
                    height: parent.height-60
                    anchors.top: labelPower1.bottom
                    anchors.centerIn: parent
                    orientation: Qt.Vertical
                    from: -100
                    to: 100
                    stepSize: 1

                    onValueChanged: {
                        labelPower1.text = "Power: "+value+"%"
                        bledevice.setPower(0, value)
                    }
                }

                Button {
                    Material.foreground: "white"
                    Material.background: "red"
                    id: stop1
                    anchors.top: slider1.bottom
                    width: parent.width
                    text: "STOP"
                    onClicked: {
                        slider1.value = 0;
                    }
                }

            }
            Frame{
                id: frame2
                width:background.width/4
                height: background.height

                Label {
                    id: labelBatt2
                    anchors.top: parent.top
                    width: parent.width
                    text: "Batt: ---%"
                    horizontalAlignment: "AlignHCenter"
                }
                Label {
                    id: labelPower2
                    anchors.top: labelBatt2.bottom
                    width: parent.width
                    text: "Power: ---%"
                    color: "red"
                    horizontalAlignment: "AlignHCenter"
                }

                Slider{
                    id: slider2
                    height: parent.height-60
                    anchors.top: labelPower2.bottom
                    anchors.centerIn: parent
                    orientation: Qt.Vertical
                    from: -100
                    to: 100
                    stepSize: 1

                    onValueChanged: {
                        labelPower2.text = "Power: "+value+"%"
                        bledevice.setPower(1, value)
                    }
                }

                Button {
                    Material.foreground: "white"
                    Material.background: "red"
                    id: stop2
                    anchors.top: slider2.bottom
                    width: parent.width
                    text: "STOP"
                    onClicked: {
                        slider2.value = 0;
                    }
                }
            }

            Frame{
                id: frame3
                width:background.width/4
                height: background.height

                Label {
                    id: labelBatt3
                    anchors.top: parent.top
                    width: parent.width
                    text: "Batt: ---%"
                    horizontalAlignment: "AlignHCenter"
                }
                Label {
                    id: labelPower3
                    anchors.top: labelBatt3.bottom
                    width: parent.width
                    text: "Power: ---%"
                    color: "red"
                    horizontalAlignment: "AlignHCenter"
                }

                Slider{
                    id: slider3
                    height: parent.height-60
                    anchors.top: labelPower3.bottom
                    anchors.centerIn: parent
                    orientation: Qt.Vertical
                    from: -100
                    to: 100
                    stepSize: 1

                    onValueChanged: {
                        labelPower3.text = "Power: "+value+"%"
                        bledevice.setPower(2, value)
                    }
                }

                Button {
                    Material.foreground: "white"
                    Material.background: "red"
                    id: stop3
                    anchors.top: slider3.bottom
                    width: parent.width
                    text: "STOP"
                    onClicked: {
                        slider3.value = 0;
                    }
                }
            }

            Frame{
                id: frame4
                width:background.width/4
                height: background.height

                Label {
                    id: labelBatt4
                    anchors.top: parent.top
                    width: parent.width
                    text: "Batt: ---%"
                    horizontalAlignment: "AlignHCenter"
                }
                Label {
                    id: labelPower4
                    anchors.top: labelBatt4.bottom
                    width: parent.width
                    text: "Power: ---%"
                    color: "red"
                    horizontalAlignment: "AlignHCenter"
                }

                Slider{
                    id: slider4
                    height: parent.height-60
                    anchors.top: labelPower4.bottom
                    anchors.centerIn: parent
                    orientation: Qt.Vertical
                    from: -100
                    to: 100
                    stepSize: 1

                    onValueChanged: {
                        labelPower4.text = "Power: "+value+"%"
                        bledevice.setPower(3, value)
                    }
                }

                Button {
                    Material.foreground: "white"
                    Material.background: "red"
                    id: stop4
                    anchors.top: slider4.bottom
                    width: parent.width
                    text: "STOP"

                    onClicked: {
                        slider4.value = 0;
                    }
                }
            }
        }





    }
    footer: ToolBar {

        width: parent.width

        // contentHeight: toolButtonScan.implicitHeight * 1

        Label {
            id: dataText
            anchors.right: parent.right
            anchors.rightMargin: 20
            y:16
            text: ""
        }

    }



    Connections {
        target: bledevice

        function onNewData(data, index) {



            console.log("id:", index, data)

        }

        function onBatteryLevel(level , index) {

            switch(index) {
            case 0:
                labelBatt1.text = "Batt: "+level+"%"
                break
            case 1:
                labelBatt2.text = "Batt: "+level+"%"
                break
            case 2:
                labelBatt3.text = "Batt: "+level+"%"
                break
            case 3:
                labelBatt4.text = "Batt: "+level+"%"
                break
            }


            //  console.log("id:", level, index)
        }


        function onScanningFinished() {

            listView.enabled=true
            scanButton.enabled=true
            scanButton.text="Scan"
            listView.enabled=true
            busyIndicator.running=false
            scanButton.enabled=true

            console.log("ScanningFinished")
        }

        function onConnectionStart() {

            disconnect = false
            busyIndicator.running=false
            //            comboF.enabled=true
            //            comboG.enabled=true
            //   drawer.close()
            listView.enabled = true
            //            graphAcc1.dataClear()
            //            graphAcc2.dataClear()
            //            graphAcc3.dataClear()
            //            graphAcc4.dataClear()
            //  graphMag.dataClear()
            //   refreshTimer.start()
            dataText.text = "Connection Start"
            console.log("ConnectionStart")
        }

        function onConnectionEnd() {

            disconnect = true
            scanButton.text = "Connection End - Scan again"
            scanButton.enabled = true
            //            comboF.enabled=false
            //            comboG.enabled=false
            bledevice.resetDeviceListModel()

            dataText.text = "Connection End"
            console.log("ConnectionEnd")
        }

        function onDevicesReady() {

            drawer.close()

            for(var i=0; i<4;i++) {
                bledevice.setLedColor(i, 0 , 0, 255)
                bledevice.setBatteryUpdate(i)
            }

        }

        function onStatusChange(data) {

            dataText.text = data


        }
    }

}
