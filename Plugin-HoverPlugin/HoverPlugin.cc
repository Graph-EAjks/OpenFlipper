#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include "HoverPlugin.hh"

void HoverPlugin::initializePlugin() {
    //================== Toolbox setup ===================
    // Toolbox is only used for development and debugging
    QWidget *toolbox = new QWidget();

    button = new QPushButton("&enable hovering", toolbox);
    QGridLayout *layout = new QGridLayout(toolbox);

    button->setCheckable(true);
    button->setChecked(false);

    layout->addWidget(button, 0, 0);

    connect(button, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));

    emit addToolbox(tr("Hover"), toolbox);

    emit addPickMode(pickmode_name);
    emit setPickModeMouseTracking(pickmode_name, true);
    //================== End Toolbox setup ===================
}

void HoverPlugin::slotMouseEvent(QMouseEvent *_event) {
    // remove all hovering before drawing new hovering
    for (auto _obj: PluginFunctions::objects(PluginFunctions::TARGET_OBJECTS)) {
        auto* pod = getOrMakePoD(_obj->id());
        remove_hovering(pod);
    }

    if (hovering) {
        size_t node_idx, target_idx;
        ACG::Vec3d hit_point;
        // Pick anything to find all possible objects
        if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx, target_idx, &hit_point)) {
            BaseObjectData* object;
            if (PluginFunctions::getPickedObject(node_idx, object)) {
                hover(_event, object);
            } else {
                std::cout << "getPickedObject not successful in slotMouseEvent\n";
            }
        }
    }
}

/**
 * Whenever the pickMode is changed by another plugin, we should probably do something, like change the value of the
 * previously used pickMode. Also, this might lead to hovering not working anymore, if another pickMode is set, so we
 * should probably do something about that as well.
 * TODO: Probably a lot
 */
void HoverPlugin::slotPickModeChanged(const std::string &_pickmode) {
    if (PluginFunctions::actionMode() != Viewer::PickingMode)
        return;

    if (!this->button->isChecked())
        return;

    this->previous_pickmode_name = _pickmode;
}

void HoverPlugin::slotButtonClicked() {
    if (button->isChecked()) {
        enterHoverMode();
        hovering = true;
    } else {
        leaveHoverMode();
        hovering = false;
    }
}

/**
 * Sets actionMode and pickMode name globally to allow to listen to MouseEvents.
 * Ideally, it should store all relevant data to allow a clean exit without affecting any other plugins.
 * TODO: enter hoverMode without affecting any other plugins.
 */
void HoverPlugin::enterHoverMode() {
    previous_action_mode = PluginFunctions::actionMode();
    previous_pickmode_name = PluginFunctions::pickMode();

    PluginFunctions::actionMode(Viewer::PickingMode);
    if (PluginFunctions::pickMode() == "") {
        PluginFunctions::pickMode(this->pickmode_name);
    } else
        emit setPickModeMouseTracking(previous_pickmode_name, true);

    button->setText("disable hovering");
}

/**
 * Resets actionMode and pickMode name globally to values before HoverMode was accessed.
 * Ideally, this should exits the HoverMode cleanly, whithout affecting any other plugins.
 * TODO: exit hoverMode without affecting any other plugins.
 */
void HoverPlugin::leaveHoverMode() {
// Is there something like slotActionModeChanged? It would be nice to update this->previous_action_mode
    PluginFunctions::actionMode(this->previous_action_mode);
//    PluginFunctions::pickMode(this->previous_pickmode_name);

    button->setText("enable hovering");
}

void HoverPlugin::hover(QMouseEvent *_event, BaseObjectData *object) {
    if(!object) return;

    //TODO: implement for other DataTypes
    auto type = object->dataType();
    if (type == DATA_TRIANGLE_MESH)
        hover_OM(_event);
    else
        std::cout << "HoverPlugin::hover: hovering for DataType " << object->dataType() << " not yet implemented\n";
}

void HoverPlugin::hover_OM(QMouseEvent *_event) {
    size_t node_idx, entityId;
    ACG::Vec3d hit_point;

    // Pick again to find face
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE,
                                        _event->pos(),node_idx, entityId, &hit_point)) {
        BaseObjectData *bod;
        if (PluginFunctions::getPickedObject(node_idx, bod)) {
            TriMeshObject *triMeshObject; // This should be generalized to OM
            if (PluginFunctions::getObject(bod->id(), triMeshObject)) {
                auto *pod = getOrMakePoD(bod->id());
                TriMesh *triMesh = triMeshObject->mesh(); // This should also be generalized to OM

                switch (hoveringPrimitive) {
                    case VERTEX:
                        break;
                    case EDGE:
                        hover_OM_edge(entityId, hit_point, pod, triMesh);
                        break;
                    case HALFEDGE:
                        break;
                    case FACE:
                        break;
                    default:
                        std::cout << "missing case in HoverPlugin::hover_OM\n";
                }
            } else
                std::cout << "getObject not successful in HoverPlugin::hover_OM\n";
        } else
            std::cout << "getPickedObject not successful in HoverPlugin::hover_OM\n";
    } else {
        std::cout << "HoverPlugin::hover_OM: No face found\n";
    }
}

void HoverPlugin::hover_OM_edge(size_t entityId, ACG::Vec3d &hit_point, HoverPoD *pod, TriMesh* mesh) {
    int hover_edge_id = find_closest_edge(entityId, hit_point, mesh);
    auto* lineNode = pod->getLineNode();
    lineNode->clear();
    color_hover_edge(hover_edge_id, pod, mesh);
}

/**
 * Finds closest edge to hit_point incident to hit face _fh
 * @return Id of closest edge
 */
int HoverPlugin::find_closest_edge(uint _fh, ACG::Vec3d &_hit_point, TriMesh* mesh) {

    TriMesh::FaceHandle fh = mesh->face_handle(_fh);

    if(!fh.is_valid())
        return -1;

    TriMesh::FaceHalfedgeIter fhe_it(*mesh, fh);

    TriMesh::HalfedgeHandle closest(-1);
    TriMesh::Scalar         closest_dist(-1);

    TriMesh::Point pp =(TriMesh::Point)_hit_point;

    for(; fhe_it.is_valid(); ++fhe_it) {

        TriMesh::Point lp0 = mesh->point(mesh->to_vertex_handle (*fhe_it));
        TriMesh::Point lp1 = mesh->point(mesh->from_vertex_handle(*fhe_it));

        double dist_new = ACG::Geometry::distPointLineSquared(pp, lp0, lp1);

        if(dist_new <closest_dist || closest_dist == -1) {

            // save closest Edge
            closest_dist = dist_new;
            closest = *fhe_it;
        }
    }

    TriMesh::EdgeHandle eh = mesh->edge_handle(closest);
    return eh.idx();
}

/**
 * Make edge with hover_edge_id visible
 */
void HoverPlugin::color_hover_edge(int hover_edge_id, HoverPoD *pod, TriMesh *mesh) {
    if (hover_edge_id <= -1)
        return;
    TriMesh::EdgeHandle eh = TriMesh::EdgeHandle(hover_edge_id);
    TriMesh::Color green = TriMesh::Color(0.0, 1.0, 0.0, 1.0);

    TriMesh::HalfedgeHandle heh_0 = mesh->halfedge_handle(eh, 0);
    TriMesh::HalfedgeHandle heh_1 = mesh->halfedge_handle(eh, 1);
    TriMesh::VertexHandle vh_0 = mesh->to_vertex_handle(heh_0);
    TriMesh::VertexHandle vh_1 = mesh->to_vertex_handle(heh_1);

    TriMesh::Point p_0 = mesh->point(vh_0);
    TriMesh::Point p_1 = mesh->point(vh_1);

    ACG::Vec3d v_0 = p_0 - ACG::Vec3d(0, 0, 0);
    ACG::Vec3d v_1 = p_1 - ACG::Vec3d(0, 0, 0);

    auto *lineNode = pod->getLineNode();
    lineNode->add_line(v_0, v_1);
    lineNode->add_color(green);
    lineNode->set_line_width(5);
}

void HoverPlugin::remove_hovering(HoverPoD *pod) {
    auto *lineNode = pod->getLineNode();
    lineNode->clear();
}

HoverPoD *HoverPlugin::getOrMakePoD(int id) {
    auto *pod = getPoD(id);

    if (!pod)
        pod = createPoD(id);

    return pod;
}

HoverPoD *HoverPlugin::getPoD(int id) {
    BaseObjectData *obj = nullptr;
    PluginFunctions::getObject(id, obj);
    if (!obj)
        return nullptr;
    return dynamic_cast<HoverPoD*>(obj->objectData(podName));
}

HoverPoD *HoverPlugin::createPoD(int id) {
    BaseObjectData *obj = nullptr;
    PluginFunctions::getObject(id, obj);
    if (!obj)
        return nullptr;
    auto lineNode = new ACG::SceneGraph::LineNode( ACG::SceneGraph::LineNode::LineSegmentsMode, obj->baseNode());
    auto pod = new HoverPoD(lineNode);
    obj->setObjectData(podName, pod);
    return pod;
}