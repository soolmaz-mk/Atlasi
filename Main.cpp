// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <GL/gl.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <chrono>

#include "common/shader.hpp"
#include "ReadJson.cpp"
#include "ReadEvents.cpp"

int main( void )
{
	 

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(650, 650, "Balad", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(1.0f, 1.0f, 0.8f, 0.0f);

	glEnable(GL_PROGRAM_POINT_SIZE);

	//VAO
	GLuint lineVAO;
	glGenVertexArrays(1, &lineVAO);
	glBindVertexArray(lineVAO);

	GLuint polygonVAO;
	glGenVertexArrays(1, &polygonVAO);
	glBindVertexArray(polygonVAO);

	GLuint eventVAO;
	glGenVertexArrays(1, &eventVAO);
	glBindVertexArray(eventVAO);	


	// Create and compile our GLSL program from the shaders
	//GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
	GLuint plyProgramID = LoadShaders( "PolygonVertexShader.vertexshader", "PolygonFragmentShader.fragmentshader" );
	GLuint lineProgramID = LoadShaders( "LineVertexShader.vertexshader", "LineFragmentShader.fragmentshader" );
	GLuint eventProgramID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

	tuple<vector<Event>,map<int, Object>> ev = readEvents();
	vector<Event> events = get<0>(ev);
	map<int, Object> objects = get<1>(ev);
	cout<<events.size()<<"\n";
	cout<<objects.size()<<"\n";

	tuple<vector<LineString>,vector<Polygon>> temp = read();
    vector<LineString> line = get<0>(temp);
    vector<Polygon> ply = get<1>(temp);

	int lineSize = line.size();
	GLint lineFirst[lineSize] = {0};
	GLint lineCount[lineSize] = {0};
	GLfloat lineVertexBufferData[45392]={0};
	int lineDataIndx=0;
	int lineFirstIndx=0;
    for (int i=0; i<lineSize; i++){
        LineString & l = line.at(i);
        int lineCoordSize = l.coords.size();	
		lineFirst[i]=lineFirstIndx;
		lineCount[i]=lineCoordSize;
        for(int j=0; j<lineCoordSize; j++){
            tuple<float,float> t = l.coords.at(j);
            float x = get<0>(t);
            float y = get<1>(t);
			lineVertexBufferData[lineDataIndx]=x;
			lineDataIndx++;
			lineVertexBufferData[lineDataIndx]=y;
			lineDataIndx++;
			lineFirstIndx++;
		}
	}



	int polySize = ply.size();
	GLint plyFirst[polySize] = {0};
	GLint plyCount[polySize] = {0};
	GLfloat polygonVertexBufferData[22824]={0};
	int plyDataIndx=0;
	int plyFirstIndx=0;
    for (int i=0; i<polySize; i++){
        Polygon & p = ply.at(i);
        int plyCoordSize = p.coords.size();
		plyFirst[i]=plyFirstIndx;
		plyCount[i]=plyCoordSize;
        for(int j=0; j<plyCoordSize; j++){
            tuple<float,float> t = p.coords.at(j);
            float x = get<0>(t);
            float y = get<1>(t);
			polygonVertexBufferData[plyDataIndx]=x;
			plyDataIndx++;
			polygonVertexBufferData[plyDataIndx]=y;
			plyDataIndx++;
			plyFirstIndx++;
		}
	}
	

	//VBO 
	glBindVertexArray(lineVAO);
	GLuint lineVertexBuffer;
	glGenBuffers(1, &lineVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lineVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertexBufferData), lineVertexBufferData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(polygonVAO);
	GLuint polygonVertexBuffer;
	glGenBuffers(1, &polygonVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, polygonVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(polygonVertexBufferData), polygonVertexBufferData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	vector<float> eventVertexBufferData;
	vector<float> eventColorBufferData;

	glBindVertexArray(eventVAO);
	GLuint eventVertexBuffer;
	glGenBuffers(1, &eventVertexBuffer);

	glBindVertexArray(eventVAO);
	GLuint eventColorBuffer;
	glGenBuffers(1, &eventColorBuffer);

	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 pers = glm::mat4(1.0f);
	unsigned int transformLoc;

	float xRotateAngle = 0;
	float xTrans = 0;
	float yTrans = 0;
	float zRotateAngle = 0;
	float xyScale = 1;
	float step = 0.0002;
	//float yScale = 1;

	chrono::steady_clock sc;   // create an object of `steady_clock` class
	auto start = sc.now();
	int ad=0;

	do{

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		// Use our shader
		//glUseProgram(programID);

		// 1rst attribute buffer : vertices
		
		//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){ //translateX -
			if(xTrans>-1.8)
				xTrans = xTrans+step;
		}

		if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){ //translateX +
			if(xTrans<1.8)
				xTrans = xTrans-step;
		}

		if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){ //translateY -
			if(yTrans>-1.8)
				yTrans = yTrans-step;
		}

		if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){ //translateY +
			if(yTrans<1.8)
				yTrans = yTrans+step;
		}

		if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){ //zoom-
			if(xyScale>0.2)
				xyScale = xyScale*(1/1.0002);
		}

		if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){ //zoom+
			if(xyScale<5)
				xyScale = xyScale*1.0002;
		}

		if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){ //rotateZ -
			//if(zRotateAngle>-90)
				zRotateAngle = zRotateAngle-0.005;
		}

		if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS){ //rotateZ +
			//if(zRotateAngle<90)
				zRotateAngle = zRotateAngle+0.005;
		}

		if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){ //rotateX -
			if(xRotateAngle>-90)
				xRotateAngle = xRotateAngle-0.005;
		}

		if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){ //rotateX +
			if(xRotateAngle<90)
				xRotateAngle = xRotateAngle+0.005;
		}
		


		trans = glm::mat4(1.0f);
		trans = glm::rotate(trans, glm::radians(xRotateAngle), glm::vec3(1.0, 0.0, 0.0));	
		trans = glm::translate(trans, glm::vec3(xTrans, yTrans, -1.0f));	  		
		trans = glm::rotate(trans, glm::radians(zRotateAngle), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, glm::vec3(xyScale, xyScale, 1));
		pers = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
		trans = pers*trans;
		
		
		
		
		// Draw 		
		glUseProgram(lineProgramID);
		transformLoc = glGetUniformLocation(lineProgramID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glBindVertexArray(lineVAO);
		glEnableVertexAttribArray(0);
		glMultiDrawArrays(GL_LINE_STRIP, lineFirst, lineCount, lineSize);

		glUseProgram(plyProgramID);
		transformLoc = glGetUniformLocation(plyProgramID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glBindVertexArray(polygonVAO);
		glEnableVertexAttribArray(0);
		glMultiDrawArrays(GL_TRIANGLE_FAN, plyFirst, plyCount, polySize);


		
		for(int i=events.size()-1; i>=0; i--){
		auto end = sc.now();    	
			if((std::chrono::duration_cast<std::chrono::seconds>(end-start).count()>=(events.at(events.size()-1).time))){
				Event eve = events.back();
				events.pop_back(); 
				int id = eve.id;  
				Object obj = objects[id];
				//cout<<events.size()<<"\n";
				if(eve.eventType==add){
					ad++;
					cout<<ad<<"\n";
					obj.vertexBufferIndex = eventVertexBufferData.size();
					eventVertexBufferData.push_back(get<0>(obj.coords));
					eventVertexBufferData.push_back(get<1>(obj.coords));
					eventVertexBufferData.push_back(0); //zero z for compability with eventColorBufferData
					
					eventColorBufferData.push_back(get<0>(obj.color));
					eventColorBufferData.push_back(get<1>(obj.color));
					eventColorBufferData.push_back(get<2>(obj.color));

					//load shader

					

				}
				if(eve.eventType==mov){
					int vertexIndx = obj.vertexBufferIndex;
					eventVertexBufferData.at(vertexIndx) = get<0>(eve.moveCoords);
					eventVertexBufferData.at(vertexIndx+1) = get<1>(eve.moveCoords);

				}
				if(eve.eventType==del){
					int vertexIndx = obj.vertexBufferIndex;
					eventVertexBufferData.at(vertexIndx) = -2; //to be neglected in next drawings //x
					eventVertexBufferData.at(vertexIndx+1) = -2; //y
					eventVertexBufferData.at(vertexIndx+2) = -1; //z
					objects.erase(id);
				}
			}
		}
		//VBO
		glBindVertexArray(eventVAO);				
		glBindBuffer(GL_ARRAY_BUFFER, eventVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*eventVertexBufferData.size(), eventVertexBufferData.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(eventVAO);					
		glBindBuffer(GL_ARRAY_BUFFER, eventColorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*eventColorBufferData.size(), eventColorBufferData.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//Draw
		glUseProgram(eventProgramID);
		glBindVertexArray(eventVAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		transformLoc = glGetUniformLocation(eventProgramID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glDrawArrays(GL_POINTS, 0, eventVertexBufferData.size()/3);
		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &lineVertexBuffer);
	glDeleteBuffers(1, &polygonVertexBuffer);
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteVertexArrays(1, &polygonVAO);
	//glDeleteProgram(programID);
	glUseProgram(lineProgramID);
	glUseProgram(plyProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	
	return 0;
}

