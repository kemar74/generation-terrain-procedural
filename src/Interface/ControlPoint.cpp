#include "ControlPoint.h"

std::shared_ptr<Shader> ControlPoint::base_shader = nullptr;
std::map<GrabberState, std::vector<float>> ControlPoint::GrabberStateColor = {
    {GrabberState::HIDDEN, {.0f, .0f, .0f, 0.f}},
    {GrabberState::INACTIVE, {.3f, .0f, .0f, .3f}},
    {GrabberState::ACTIVE, {.8f, .0f, .0f, .5f}}
};

ControlPoint::ControlPoint()
    : ControlPoint(Vector3())
{
    this->mesh.isDisplayed = false;
}

ControlPoint::ControlPoint(Vector3 pos, float radius, GrabberState state, bool useTheManipulatedFrame)
    : position(pos), radius(radius), state(state), useManipFrame(useTheManipulatedFrame), shape(radius, position, 10, 10)
{
    this->mesh = Mesh((ControlPoint::base_shader ? std::make_shared<Shader>(*ControlPoint::base_shader) : nullptr), true);
    this->move(pos);
    this->prevPosition = pos;
    if (!useManipFrame) {
        this->manipFrame.removeFromMouseGrabberPool();
    } else {
        this->manipFrame.addInMouseGrabberPool();
    }

    QObject::connect(&this->manipFrame, &qglviewer::ManipulatedFrame::modified, this, [=](){
        Q_EMIT this->modified();
        this->updateStateDependingOnManipFrame();
    });
}

ControlPoint::~ControlPoint()
{
    this->manipFrame.removeFromMouseGrabberPool();
}

void ControlPoint::setState(GrabberState newState)
{
    this->state = newState;
}

void ControlPoint::updateStateDependingOnManipFrame()
{
    if (this->useManipFrame)
        if (this->state != HIDDEN)
            this->setState(this->manipFrame.isManipulated() ? ACTIVE : INACTIVE);
}

void ControlPoint::onUpdate(std::function<void ()> func)
{
    this->onUpdateCallback = func;
}

void ControlPoint::afterUpdate(std::function<void ()> func)
{
    this->afterUpdateCallback = func;
}

void ControlPoint::updateSphere()
{
    if (this->useManipFrame)
        this->position = Vector3(this->manipFrame.position().x, this->manipFrame.position().y, this->manipFrame.position().z);

    if(this->onUpdateCallback) { // && this->useManipFrame && this->manipFrame.isManipulated())
        if (prevPosition != position) {
            this->prevPosition = position;
            this->onUpdateCallback();
        }
    }
    if (this->afterUpdateCallback) {
        if (this->useManipFrame && (this->manipFrame.isManipulated() == false && this->currentlyManipulated == true))
            this->afterUpdateCallback();
    }
    this->currentlyManipulated = this->manipFrame.isManipulated();

    this->shape.position = this->position;
    this->shape.radius = this->radius;
    this->shape.buildVerticesFlat();
    this->mesh.fromArray(this->shape.mesh.vertexArrayFloat);
    this->mesh.update();
}

void ControlPoint::display()
{
    if (this->state == HIDDEN) return;
    else {
        if (this->useManipFrame)
            this->setState(this->manipFrame.isManipulated() ? ACTIVE : INACTIVE);

        this->mesh.shader->setVector("color", ControlPoint::GrabberStateColor[this->state]);
        this->updateSphere();
        this->mesh.display();
    }
}

void ControlPoint::move(Vector3 newPos)
{
    if (this->useManipFrame) {
        this->manipFrame.setPosition(this->position.x, this->position.y, this->position.z);
    }
    this->position = newPos;
    this->updateSphere();
}
