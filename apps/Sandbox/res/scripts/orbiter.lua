local node = 
{
    speed = 10.0
}

function node:begin()

end

-- @param target Entity
-- Set the target entity to orbit around
function node:setTarget(target)
    self.target = target;
end
-- @param delta float
-- Update function called every frame
function node:update(delta)
    if self.target ~= nil then
        self:orbit_around_target(delta);
    end
end
-- @param delta float
-- Orbit the entity around the target entity
function node:orbit_around_target(delta)
    local pos = self.entity:getPosition();
    local targetPos = self.target:getPosition();

    local direction = targetPos - pos;
    local distance = direction:length();
    direction:normalize();

    local right = direction:cross(Vector3(0,1,0));
    local up = right:cross(direction);

    local angle = self.speed * delta;
    local rotation = Quaternion();
    rotation:fromAxisAngle(up, angle);

    local newDirection = rotation * direction;
    local newPos = targetPos - newDirection * distance;

    self.entity:setPosition(newPos);
    self.entity:lookAt(targetPos, Vector3(0,1,0));
end