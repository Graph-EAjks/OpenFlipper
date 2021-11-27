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
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
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
    void simple_hover(QMouseEvent *_event);

    int find_closest_edge(uint _fh, ACG::Vec3d &_hit_point, TriMesh *mesh);

    void remove_hovering(HoverPoD *pod);

    HoverPoD* getOrMakePoD(int id);
    HoverPoD* getPoD(int id);
    HoverPoD* createPoD(int id);

    enum hovering_primitive {
        VERTEX, EDGE, HALFEDGE, FACE
    };

    QPushButton* button;
    const std::string pickmode_name = "HoverPlugin";

    bool coolHovering = false;
    bool hovering = true;
    hovering_primitive hoveringPrimitive = EDGE;

    std::string previous_pickmode_name = "";
    Viewer::ActionMode previous_action_mode = Viewer::PickingMode;

    QString podName = "HoverPluginPoD";

    /*============================*\
     *          Templates         *
    \*============================*/
    template <typename T>
    void hover_OM(BaseObjectData *bod, ACG::Vec3d hit_point, size_t entityId, T *meshObject) {

        auto *pod = getOrMakePoD(bod->id());
        auto *mesh = meshObject->mesh();
        switch (hoveringPrimitive) {
            case VERTEX:
                break;
            case EDGE:
                hover_OM_edge(entityId, hit_point, pod, mesh);
                break;
            case HALFEDGE:
                break;
            case FACE:
                break;
            default:
                std::cout << "missing case in HoverPlugin::hover_OM\n";
        }
    }

    template <typename T>
    void hover_OM_edge(size_t entityId, ACG::Vec3d &hit_point, HoverPoD *pod, T *mesh) {

        int hover_edge_id = find_closest_edge(entityId, hit_point, mesh);
        auto *lineNode = pod->getLineNode();
        lineNode->clear();
        color_hover_edge(hover_edge_id, pod, mesh);
    }

    /**
    * Finds closest edge to hit_point incident to hit face _fh
    * @return Id of closest edge
    */
    template <typename T>
    int find_closest_edge(size_t _fh, ACG::Vec3d &_hit_point, T *mesh) {

        typename T::FaceHandle fh = mesh->face_handle(_fh);

        if(!fh.is_valid())
            return -1;

        typename T::FaceHalfedgeIter fhe_it(*mesh, fh);

        typename T::HalfedgeHandle closest(-1);
        typename T::Scalar         closest_dist(-1);

        typename T::Point pp =(typename T::Point)_hit_point;

        for(; fhe_it.is_valid(); ++fhe_it) {

            typename T::Point lp0 = mesh->point(mesh->to_vertex_handle (*fhe_it));
            typename T::Point lp1 = mesh->point(mesh->from_vertex_handle(*fhe_it));

            double dist_new = ACG::Geometry::distPointLineSquared(pp, lp0, lp1);

            if(dist_new <closest_dist || closest_dist == -1) {

                // save closest Edge
                closest_dist = dist_new;
                closest = *fhe_it;
            }
        }

        typename T::EdgeHandle eh = mesh->edge_handle(closest);
        return eh.idx();
    }

    /**
    * Make edge with hover_edge_id visible
    */
    template <typename T>
    void color_hover_edge(int hover_edge_id, HoverPoD *pod, T *mesh) {
        if (hover_edge_id <= -1)
            return;
        typename T::EdgeHandle eh = typename T::EdgeHandle(hover_edge_id);
        typename T::Color green = typename T::Color(0.0, 1.0, 0.0, 1.0);

        typename T::HalfedgeHandle heh_0 = mesh->halfedge_handle(eh, 0);
        typename T::HalfedgeHandle heh_1 = mesh->halfedge_handle(eh, 1);
        typename T::VertexHandle vh_0 = mesh->to_vertex_handle(heh_0);
        typename T::VertexHandle vh_1 = mesh->to_vertex_handle(heh_1);

        typename T::Point p_0 = mesh->point(vh_0);
        typename T::Point p_1 = mesh->point(vh_1);

        ACG::Vec3d v_0 = p_0 - ACG::Vec3d(0, 0, 0);
        ACG::Vec3d v_1 = p_1 - ACG::Vec3d(0, 0, 0);

        auto *lineNode = pod->getLineNode();
        lineNode->add_line(v_0, v_1);
        lineNode->add_color(green);
        lineNode->set_line_width(5);
    }

    template <typename T>
    void simple_hover_color(int entityId, BaseObjectData *bod, T *meshObject) {
        if (PluginFunctions::getObject(bod->id(), meshObject)) {
            auto *pod = getOrMakePoD(bod->id());
            auto *mesh = meshObject->mesh();
            color_hover_edge(entityId, pod, mesh);
        }
    }
};
#endif