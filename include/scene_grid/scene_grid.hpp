#ifndef __SCENE_GRID_HPP__
#define __SCENE_GRID_HPP__

class SceneGrid {
public:
    SceneGrid() {}

    virtual ~SceneGrid() {}
    
    virtual void dispatch_compute() const = 0;
};

#endif
