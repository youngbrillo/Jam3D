#include "entity.hpp"
#include <raylib.h>
#include "jam/modules/core3d/components/transform3d.hpp"
//#include "Jam/modules/physics/components/physColliders.hpp"

#ifdef JAM_ENTITY_HPP
using namespace jam;
using namespace jam::components;

jam::Entity::Entity(entt::entity handle, entt::registry& regsitry)
    : mHandle(handle)
    , mWorld(&regsitry)
{
}

jam::Entity::Entity(entt::entity handle, entt::registry* regsitry)
    : mHandle(handle)
    , mWorld(regsitry)
{
}

void jam::Entity::destroy()
{
    addOrReplace<components::DeleteTag>();
    while (getFirstChild())
    {
        Entity child = getFirstChild();
        child.destroy();
    }

    removeParent();
}

std::string jam::Entity::getName()
{
    if (auto* nameTag = tryGet<components::NameTag>(); nameTag)
    {
        return nameTag->name;
    }
    return "no entity name provided";
}

UUID jam::Entity::getUUID()
{
    return get<components::IDTag>().tag;
}
Vector3 jam::Entity::getLocalPosition()
{
    return get<components::Transform3D>().position;

}
Vector3 jam::Entity::getWorldPosition()
{
    Vector3 position = get<components::Transform3D>().position;


    return position;
}
Matrix jam::Entity::GetLocalMatrix()
{
    return get<components::Transform3D>().toMatrix();
}
Matrix jam::Entity::GetWorldMatrix()
{
    Matrix mat = GetLocalMatrix();
    return mat;
}
void jam::Entity::setPosition(Vector3 position)
{
    get<components::Transform3D>().position = position;

    //if has rigidbody component....

}

bool jam::Entity::isEnabled()
{
    return has<components::DisabledTag>() == false;
}

void jam::Entity::enable()
{
    remove<components::DisabledTag>();
}

void jam::Entity::disable()
{
    addOrReplace<components::DisabledTag>();
}

bool jam::Entity::isVisible()
{
    return has<components::HiddenTag>() == false;
}

void jam::Entity::show()
{
    remove<components::HiddenTag>();
}

void jam::Entity::hide()
{
    addOrReplace<components::HiddenTag>();
}

bool jam::Entity::isSerializable()
{
    return has<components::DoNotSerializeTag>() == false;
}

void jam::Entity::disableSerialization()
{
    addOrReplace<components::DoNotSerializeTag>();
}

void jam::Entity::enableSerialization()
{
    remove<components::DoNotSerializeTag>();
}


Entity jam::Entity::createEntity()
{
    Entity e = Entity(mWorld->create(), mWorld);

    e.add<components::NameTag>().name = "entity";
    e.add<components::IDTag>().tag = UUID();
    e.add<components::Relationship>();
    return e;
}

Entity jam::Entity::createChild(std::string name)
{
    Entity e = createEntity();
    e.get<components::NameTag>().name = name;

    this->addChild(e);

    return e;
}

void jam::Entity::addParent(Entity parent)
{
    using namespace jam::components;

    Relationship& r = get<Relationship>();
    if (r.parent == parent)
        return;
    if (r.parent != entt::null)
        this->removeParent(); //handles all delinking
    parent.addChild(*this);
}

void jam::Entity::removeParent()
{
    using namespace jam::components;
    Relationship& r = get<Relationship>();
    if (r.parent == entt::null)
        return;

    Entity parent(r.parent, mWorld);
    parent.removeChild(*this); //handles sibling delinking
    r.parent = entt::null;
}

void jam::Entity::addChild(Entity child)
{
    using namespace jam::components;
    Relationship& r = get<Relationship>();
    if (hasChild(child) || child == *this)
        return; //don't add duplicate children! and you cannot be a child of yourself!

    Relationship& childR = child.get<Relationship>();
    if (child.getParent())
        child.removeParent();

    childR.parent = mHandle;

    entt::entity it = r.firstChild;
    if (it == entt::null) //no children
    {
        r.firstChild = child;
    }
    else //not the first child, add it to the end of the linked list
    {
        while (it != entt::null)
        {
            Entity sib(it, mWorld);
            auto& nr = sib.get<Relationship>();
            if (nr.nextSib == entt::null)
            {
                nr.nextSib = child;
                childR.prevSib = sib;
                it = entt::null;
            }
            else
                it = nr.nextSib;
        }
    }

}

void jam::Entity::removeChild(Entity child)
{
    using namespace jam::components;

    Relationship& pr = get<Relationship>();
    if (!hasChild(child))
        return;

    Relationship& r = child.get<Relationship>();

    if (pr.firstChild == child)
    {
        pr.firstChild = r.nextSib;
    }

    if (r.prevSib != entt::null)
    {
        Relationship& pr = child.getPrevSibling().get<Relationship>();
        pr.nextSib = r.nextSib;
    }
    if (r.nextSib != entt::null)
    {
        Relationship& nr = child.getNextSibling().get<Relationship>();
        nr.prevSib = r.prevSib;
    }

    r.prevSib = entt::null;
    r.nextSib = entt::null;
    r.parent = entt::null;
}

void jam::Entity::removeAllChildren()
{
    using namespace jam::components;
    for (Entity i = getFirstChild(); i.isValid(); getFirstChild())
    {
        removeChild(i);
    }
}

Entity jam::Entity::getFirstChild()
{
    using namespace jam::components;
    return Entity(get<Relationship>().firstChild, mWorld);
}

Entity jam::Entity::getNextSibling()
{
    using namespace jam::components;
    return Entity(get<Relationship>().nextSib, mWorld);
}

Entity jam::Entity::getPrevSibling()
{
    using namespace jam::components;
    return Entity(get<Relationship>().prevSib, mWorld);
}

Entity jam::Entity::getParent()
{
    using namespace jam::components;
    return Entity(get<Relationship>().parent, mWorld);
}

bool jam::Entity::hasChild(Entity child)
{
    using namespace jam::components;

    Entity it = getFirstChild();
    while (it)
    {
        if (it == child)
            return true;
        it = it.getNextSibling();
    }
    return false;
}

bool jam::Entity::isDecendantOf(Entity ancestor)
{
    using namespace jam::components;

    while (Entity family = getParent())
    {
        if (family == ancestor)
            return true;

        family = family.getParent();
    }
    return false;
}

bool jam::Entity::hasParent()
{
    using namespace jam::components;
    Relationship& r = get<Relationship>();
    return r.parent != entt::null;
}

uint32_t jam::Entity::getNumberOfChildren()
{
    return 0;
}
#endif