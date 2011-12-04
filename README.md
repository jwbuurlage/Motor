Motor -- graphics engine
========================
Introduction
------------

Motor is a project by two undergrad students at the University of Utrecht.


Object tree
-----------

> __root.cpp__: Entry point, contains many _root_ class such as `listener`. 

> __Input__: Objects can add themselves as a listener to a keyevent/mouseevent using something along the lines of a responder chain. 

> __World__: Maintains `scenes` which on their turn have a list of `entities` (objects, lights, ...), and hold information on the atmosphere etc. 

> __Renderer__: Holds a reference to the object-list contained in the `world` object, and renders the current scene to the proper viewport.