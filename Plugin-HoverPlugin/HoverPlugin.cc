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
    //================== End Toolbox setup ===================

    emit addPickMode(pickmode_name);
    emit setPickModeMouseTracking(pickmode_name, true);
}

void HoverPlugin::slotMouseEvent(QMouseEvent *_event) {
    // remove all hovering before drawing new hovering
    for (auto _obj: PluginFunctions::objects()) {
        auto* pod = getOrMakePoD(_obj->id());
        remove_hovering(pod);
    }

    if (hovering) {
        size_t node_idx, target_idx;
        ACG::Vec3d hit_point;
        // Pick anything to find all possible objects
        if (coolHovering) {
            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(), node_idx, target_idx,
                                                &hit_point)) {
                BaseObjectData *object;
                if (PluginFunctions::getPickedObject(node_idx, object)) {
                    hover(_event, object);
                } else {
                    std::cout << "getPickedObject not successful in slotMouseEvent\n";
                }
            }
        } else {
            simple_hover(_event);
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

    if (!hovering)
        return;

    previous_pickmode_name = _pickmode;
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
//    PluginFunctions::actionMode(this->previous_action_mode);
//    PluginFunctions::pickMode(this->previous_pickmode_name);

    button->setText("enable hovering");
}

/**
 * Highlights entities just as simple_hover(), but selects the entity on the top face closest to the mouse courser. This
 * looks way cooler than the other version, as there is always something highlighted, if the mouse hovers over anything.
 */
void HoverPlugin::hover(QMouseEvent *_event, BaseObjectData *object) {
    if(!object) return;
    size_t node_idx, entityId;
    ACG::Vec3d hit_point;

    //TODO: implement for other DataTypes
    auto type = object->dataType();
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, entityId,
                                        &hit_point)) {
        BaseObjectData *bod;
        if (PluginFunctions::getPickedObject(node_idx, bod)) {
            if (type == DATA_TRIANGLE_MESH) {
                TriMeshObject *triMeshObject;
                if (PluginFunctions::getObject(bod->id(), triMeshObject)) {
                    hover_OM(bod, hit_point, entityId, triMeshObject);
                } else
                    std::cout << "hoverPlugin::hover: getObject failed for Triangle Meshes.\n";
            } else if (type == DATA_POLY_MESH) {
                PolyMeshObject *polyMeshObject;
                if (PluginFunctions::getObject(bod->id(), polyMeshObject))
                    hover_OM(bod, hit_point, entityId, polyMeshObject);
            } else
                std::cout << "HoverPlugin::hover: hovering for DataType " << object->dataType() << " not yet implemented\n";
        }
    } else
        std::cout << "HoverPlugin::hover: scenegraphPick not successful.\n";
}

/**
 * Highlights entities just as hover(), but highlights only the entity, where the mouse courser is exactly on top.
 */
void HoverPlugin::simple_hover(QMouseEvent *_event) {

    switch (hoveringPrimitive) {
        case VERTEX:
            break;
        case EDGE: {
            size_t node_idx, entityId;
            ACG::Vec3d hit_point;
            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_EDGE, _event->pos(), node_idx, entityId,
                                                &hit_point)) {
                BaseObjectData *bod;
                if (PluginFunctions::getPickedObject(node_idx, bod)) {
                    auto type = bod->dataType();
                    if (type == DATA_TRIANGLE_MESH) {
                        TriMeshObject *triMeshObject;
                        simple_hover_color(entityId, bod, triMeshObject);
                    } else if (type == DATA_POLY_MESH) {
                        PolyMeshObject *polyMeshObject;
                        simple_hover_color(entityId, bod, polyMeshObject);
                    }
                }
            }
            break;
        }
        case HALFEDGE:
            break;
        case FACE:
            break;
        default:
            break;
    }
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