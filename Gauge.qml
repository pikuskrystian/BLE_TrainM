import QtQuick
//import Qt5Compat.GraphicalEffects

Item {
    id: root
    width: size
    height: size
    property real from: -100
    property real to: 100
    property real valueX: 0
    property real valueY: 0
    property real valueZ: 0
    property real valueA: 0
    property string quantity: ""
    property string unit: ""
    property int size: 200
    property real arcBeginX: -90
    property real arcEndX: -90
    property real arcBeginY: -90
    property real arcEndY: -90
    property real arcBeginZ: -90
    property real arcEndZ: -90
    property real arcBeginA: -90
    property real arcEndA: -90
    property bool showBackground: true
    property bool showGlow: false
    property real lineWidth: 5
    property color colorCircleA: "#00FFFF"
    property color colorCircleX: "#FF8080"
    property color colorCircleY: "#00FF80"
    property color colorCircleZ: "#C0FF80"

    property color colorBackground: "#000000"

    onValueXChanged: {

        if(valueX >= 0) {

            arcBeginX= -90
            arcEndX = (valueX*360/Math.abs(to-from))-90
        } else {

            arcEndX= -90
            arcBeginX = (valueX*360/Math.abs(to-from))-90
        }
        absCalculate(valueX, valueY, valueZ)
        canvas.requestPaint()
    }
    onValueYChanged: {

        if(valueY >= 0) {

            arcBeginY= -90
            arcEndY = (valueY*360/Math.abs(to-from))-90
        } else {

            arcEndY= -90
            arcBeginY = (valueY*360/Math.abs(to-from))-90
        }
        absCalculate(valueX, valueY, valueZ)
        canvas.requestPaint()
    }

    onValueZChanged: {

        if(valueZ >= 0) {

            arcBeginZ= -90
            arcEndZ = (valueZ*360/Math.abs(to-from))-90
        } else {

            arcEndZ= -90
            arcBeginZ = (valueZ*360/Math.abs(to-from))-90
        }
        absCalculate(valueX, valueY, valueZ)
        canvas.requestPaint()
    }

    //    Behavior on valueA {
    //        id: animationArcEnd
    //        enabled: true
    //        NumberAnimation {
    //            duration: 100
    //            easing.type: Easing.InOutCubic
    //        }
    //    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            root.valueX=to
            root.valueY=to
            root.valueZ=to

        }
        onReleased : {
            root.valueX=0
            root.valueY=0
            root.valueZ=0
        }
    }

    Canvas {
        id: canvas
        anchors.fill: root
        onPaint: {
            var ctx = getContext("2d")
            var x = width / 2
            var y = height / 2
            var startX = Math.PI * (parent.arcBeginX / 180)
            var endX = Math.PI * (parent.arcEndX / 180)
            var startY = Math.PI * (parent.arcBeginY / 180)
            var endY = Math.PI * (parent.arcEndY / 180)
            var startZ = Math.PI * (parent.arcBeginZ / 180)
            var endZ = Math.PI * (parent.arcEndZ / 180)
            var startA = Math.PI * (parent.arcBeginA / 180)
            var endA = Math.PI * (parent.arcEndA / 180)
            ctx.reset()
            ctx.beginPath()
            ctx.fillStyle = root.colorCircleA
            ctx.textAlign="center"

            ctx.font = (root.size / 12)+"px Arial"
            ctx.textBaseline="middle"
            ctx.fillText(quantity, x, y - (root.size / 6))

            ctx.font = "bold "+(root.size / 5)+"px Arial"
            ctx.textBaseline="middle"
            ctx.fillText(root.valueA.toFixed(1), x, y)

            ctx.font = (root.size / 12)+"px Arial"
            ctx.textBaseline="middle"
            ctx.fillText(unit, x, y + (root.size / 6))

            ctx.stroke()
            if (root.showBackground) {
                ctx.beginPath()
                ctx.arc(x, y, (width / 2) - (parent.lineWidth/2) - 4*lineWidth, 0, Math.PI * 2, false)
                ctx.lineWidth = root.lineWidth*5
                ctx.strokeStyle = root.colorBackground
                ctx.stroke()
            }
            ctx.beginPath()
            ctx.arc(x, y, (width / 2) - (parent.lineWidth / 2) - 2.5*lineWidth, startA, endA, false)
            ctx.lineWidth = 2*root.lineWidth
            ctx.strokeStyle = root.colorCircleA
            ctx.stroke()

            ctx.beginPath()
            ctx.arc(x, y, (width / 2) - (parent.lineWidth/2) - 4*lineWidth, startX, endX, false)
            ctx.lineWidth = root.lineWidth
            ctx.strokeStyle = root.colorCircleX
            ctx.stroke()

            ctx.beginPath()
            ctx.arc(x, y, (width / 2) - (parent.lineWidth / 2) - 5*lineWidth, startY, endY, false)
            ctx.lineWidth = root.lineWidth
            ctx.strokeStyle = root.colorCircleY
            ctx.stroke()

            ctx.beginPath()
            ctx.arc(x, y, (width / 2) - (parent.lineWidth / 2) - 6*lineWidth, startZ, endZ, false)
            ctx.lineWidth = root.lineWidth
            ctx.strokeStyle = root.colorCircleZ
            ctx.stroke()


        }
    }

//    Glow {
//        id:glow
//        visible: showGlow
//        anchors.fill: root
//        radius: 5
//        spread: 0.0
//        color: colorCircleA
//        source: canvas
//    }

    function absCalculate(x, y, z) {

        valueA = Math.sqrt(x*x + y*y + z*z)
        var m = Math.sqrt(to*to + to*to + to*to)


        //        arcBeginA = (valueA*360/m)-105
        //        arcEndA = (valueA*360/m)-75

        arcEndA = (valueA*360/m)-90

    }
}
