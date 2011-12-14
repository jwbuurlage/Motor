/* This document is part of the Motor project,
 * an experimental graphics engine.
 *
 *	Jan-Willem Buurlage, 	j.buurlage@students.uu.nl. 
 *	Tom Bannink, 			t.bannink@students.uu.nl
 *
 * http://www.github.com/jwbuurlage/Motor 
 * 
 */
#pragma once


namespace Motor {
	class Renderer;
	
	class Root {
	public:
		Root();
		~Root();
		
		int initialize();
		void cleanup();

		void startRendering(); //use with FrameListeners
		bool renderOneFrame();

	private:
		Renderer* const renderer;
	};

}