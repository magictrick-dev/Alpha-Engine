#pragma once

class SceneInterface
{

    protected:
        virtual bool mount() = 0;
        virtual bool unmount() = 0;
        virtual bool initialize() = 0;
        virtual bool execute() = 0;

};

class LayerInterface
{
    public:
        virtual bool initialize() = 0;
        virtual bool deinitialize() = 0;
        virtual bool update(float dt) = 0;
        virtual bool fixed_update(float dt) = 0;
        virtual void render() = 0;

};

