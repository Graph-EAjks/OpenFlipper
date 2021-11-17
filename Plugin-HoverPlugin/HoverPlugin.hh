#ifndef HOVERPLUGIN_HH_INCLUDED
#define HOVERPLUGIN_HH_INCLUDED

#include "QPushButton"
#include "QLabel"
#include "QGridLayout"

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>

#include <ACG/Scenegraph/LineNode.hh>

#include <ObjectTypes/TriangleMesh/TriangleMeshTypes.hh>
#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

class HoverPoD:
        public QObject,
        public PerObjectData {
Q_OBJECT

public:
    HoverPoD(ACG::SceneGraph::LineNode *lineNode) {
        this->lineNode = lineNode;
    }

    ACG::SceneGraph::LineNode* getLineNode() {
        return this->lineNode;
    }

private:
    ACG::SceneGraph::LineNode* lineNode;
};

class HoverPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, MouseInterface, PickingInterface {
    Q_OBJECT
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(ToolboxInterface)
    Q_INTERFACES(LoggingInterface)
    Q_INTERFACES(MouseInterface)
    Q_INTERFACES(PickingInterface)
    Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.examples.HoverPlugin")

public:
    // BaseInterface
    QString name() { return QString("HoverPlugin"); };
    QString description() { return QString("enables preview highlighting while hovering over entities"); };

signals:
    // BaseInterface
    void updateView();

    // ToolboxInterface
    void addToolbox(QString _name, QWidget* _widget);

    // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    // PickingInterface
    void addPickMode (const std::string &_mode);
    void setPickModeMouseTracking (const std::string &_mode, bool _mouseTracking);

private slots:
    // BaseInterface
    void initializePlugin();

    // MouseInterface
    void slotMouseEvent(QMouseEvent* _event);

    // PickingInterface
    void slotPickModeChanged(const std::string& _pickmode);

    void slotButtonClicked();

public slots:
    QString version() { return QString("0.1"); };

private:
    void enterHoverMode();
    void leaveHoverMode();

    void hover(QMouseEvent *_event, BaseObjectData *object);
    void hover_OM(QMouseEvent *_event);
    void hover_OM_edge(size_t entityId, ACG::Vec3d &hit_point, HoverPoD *pod, TriMesh *mesh);

    int find_closest_edge(uint _fh, ACG::Vec3d &_hit_point, TriMesh *mesh);

    void color_hover_edge(int hover_edge_id, HoverPoD *pod, TriMesh *mesh);
    void remove_hovering(HoverPoD *pod);

    HoverPoD* getOrMakePoD(int id);
    HoverPoD* getPoD(int id);
    HoverPoD* createPoD(int id);

    enum hovering_primitive {
        VERTEX, EDGE, HALFEDGE, FACE
    };

    QPushButton* button;
    const std::string pickmode_name = "HoverPlugin";

    bool hovering = false;
    hovering_primitive hoveringPrimitive = EDGE;

    std::string previous_pickmode_name = "";
    Viewer::ActionMode previous_action_mode = Viewer::PickingMode;

    QString podName = "HoverPluginPoD";
};
#endif