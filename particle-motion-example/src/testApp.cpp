#include "testApp.h"
class Ribbon
{
public:
	void setup()
	{
		rmesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP); 
		points.assign(10, ofVec3f() );
		for(int i = 0; i < points.size(); i++){
			rmesh.addVertex(ofVec3f(0,0,0));
			rmesh.addVertex(ofVec3f(0,0,0));
			float revertIndex = points.size()-i;
			rmesh.addColor(ofFloatColor(0,(float)revertIndex/points.size(),(float)revertIndex/points.size(),(float)revertIndex/points.size()));
			rmesh.addColor(ofFloatColor(0,(float)revertIndex/points.size(),(float)revertIndex/points.size(),(float)revertIndex/points.size()));
		}
	}
	void update(ofVec3f p)
	{
		points[0].set(p);
		for(int _i = points.size()-1; _i > 0; _i--){
			points[_i]  = points[_i-1];
			points[_i].z = _i*-10;
		}
		for(int i = 0; i < points.size(); i++){
			ofVec3f thisPoint;
			ofVec3f nextPoint;
			//find this point and the next point
			if(i>1)
			{

				thisPoint = points[i-1];
				nextPoint = points[i];
			}

			//get the direction from one to the next. 
			//the ribbon should fan out from this direction
			ofVec3f direction = (nextPoint - thisPoint);

			//get the distance from one point to the next
			float distance = direction.length()/2;

			//get the normalized direction. normalized vectors always have a length of one
			//and are really useful for representing directions as opposed to something with length
			ofVec3f unitDirection = direction.normalized();

			//find both directions to the left and to the right 
			ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,1,0));
			ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,1,0));

			//use the map function to determine the distance.
			//the longer the distance, the narrower the line. 
			//this makes it look a bit like brush strokes
			float thickness = ofMap(sin(distance/direction.length()), 0, 100, 0, 100, true);

			//calculate the points to the left and to the right
			//by extending the current point in the direction of left/right by the length
			ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
			ofVec3f rightPoint = thisPoint+toTheRight*thickness;

			//add these points to the triangle strip

			rmesh.setVertex(i*2,ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
			rmesh.setVertex(i*2+1,ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
		}
	}
	void draw()
	{
		glEnable(GL_DEPTH_TEST);
		rmesh.draw();
		glDisable(GL_DEPTH_TEST);
	}
	vector<ofVec3f> points;
	ofMesh rmesh;
};
class Particle
{
public:
	
	ofVec3f pos;
	ofVec3f vel;
	ofVec3f force;
};
#define RIBBON_SIZE 50
class Tracker
{
public:
	
	const ofxBvhJoint *joint, *root;
	deque<ofVec3f> samples;
	//Ribbon ribbon;
	ofMesh rmesh;
	void setup(const ofxBvhJoint *o, const ofxBvhJoint *r , int actorIndex)
	{
		joint = o;
		root = r;
		//ribbon.setup();
		rmesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP); 
		
		for(int i = 0; i < RIBBON_SIZE; i++){
			rmesh.addVertex(ofVec3f(0,0,-i*50));
			rmesh.addVertex(ofVec3f(0,0,-i*50));
			float revertIndex = RIBBON_SIZE-i;
			float c  = (float)revertIndex/RIBBON_SIZE;
			rmesh.addColor(ofFloatColor((actorIndex==0)?c:0,(actorIndex==1)?c:0,(actorIndex==2)?c:0,(float)revertIndex/RIBBON_SIZE));
			rmesh.addColor(ofFloatColor((actorIndex==0)?c:0,(actorIndex==1)?c:0,(actorIndex==2)?c:0,(float)revertIndex/RIBBON_SIZE));
		}
	}
	
	void update()//vector<Particle>& particles)
	{
		const ofVec3f &p = joint->getPosition();
		//ribbon.update(joint->getPosition());
		// update sample
		{
			samples.push_front(joint->getPosition());
			while (samples.size() > RIBBON_SIZE)
				samples.pop_back();
		}
		float times = sin(ofGetElapsedTimef()*0.0001);
		float timec = cos(ofGetElapsedTimef()*0.0001);
		for(int i = 0; i < samples.size(); i++){
			ofVec3f thisPoint;
			ofVec3f nextPoint;
			//find this point and the next point
			if(i>1)
			{

				thisPoint = samples[i-1];
				nextPoint = samples[i];
				thisPoint.z+=timec*i*10;
				nextPoint.z+=timec*i*10;
				thisPoint.x+=times*i*10;
				nextPoint.x+=times*i*10;
			}

			//get the direction from one to the next. 
			//the ribbon should fan out from this direction
			ofVec3f direction = (nextPoint - thisPoint);

			//get the distance from one point to the next
			float distance = direction.length()/2;

			//get the normalized direction. normalized vectors always have a length of one
			//and are really useful for representing directions as opposed to something with length
			ofVec3f unitDirection = direction.normalized();

			//find both directions to the left and to the right 
			ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,1,0));
			ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,1,0));

			//use the map function to determine the distance.
			//the longer the distance, the narrower the line. 
			//this makes it look a bit like brush strokes
			float thickness = ofMap(sin(distance/direction.length()), 0, 1, 10, 20, true);

			//calculate the points to the left and to the right
			//by extending the current point in the direction of left/right by the length
			ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
			ofVec3f rightPoint = thisPoint+toTheRight*thickness;

			//add these points to the triangle strip

			rmesh.setVertex(i*2,ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
			rmesh.setVertex(i*2+1,ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
		}
		// update particle force
		/*{
			const float n = 2.0;
			const float A = 0.4;
			const float m = 1.1;
			const float B = 1.6;
			
			for (int i = 0; i < particles.size(); i++)
			{
				Particle &o = particles[i];
				ofVec3f dist = (o.pos - p);
				float r = dist.squareLength();
				
				if (r > 0 && r < 30*30)
				{
					r = sqrt(r);
					dist /= r;
					
					o.force += ((A / pow(r, n)) - (B / pow(r, m))) * dist * 2;
				}
			}
		}*/
	}
	
	float length()
	{
		if (samples.empty()) return 0;
		
		float v = 0;
		for (int i = 0; i < samples.size() - 1; i++)
			v += samples[i].distance(samples[i + 1]);
		
		return v;
	}
	
	float dot()
	{
		if (samples.empty()) return 0;
		
		float v = 0;
		
		for (int i = 1; i < samples.size() - 1; i++)
		{
			const ofVec3f &v0 = samples[i - 1];
			const ofVec3f &v1 = samples[i];
			const ofVec3f &v2 = samples[i + 1];
			
			if (v0.squareDistance(v1) == 0) continue;
			if (v1.squareDistance(v2) == 0) continue;
			
			const ofVec3f d0 = (v0 - v1).normalized();
			const ofVec3f d1 = (v1 - v2).normalized();
			
			v += (d0).dot(d1);
		}
		
		return v / ((float)samples.size() - 2);
	}
	
	void draw()
	{
		/*float len = length();
		len = ofMap(len, 30, 40, 0, 1, true);
		
		float d = dot();
		d = ofMap(d, 1, 0, 255, 0, true);*/
		rmesh.draw();
		/*glBegin(GL_LINE_STRIP);
		for (int i = 0; i < samples.size(); i++)
		{
			float a = ofMap(i, 0, samples.size() - 1, 1, 0, true);
			ofSetColor(d * len, 140 * a);
			glVertex3fv(samples[i].getPtr());
		}
		glEnd();*/
		//ribbon.draw();
	}
};

class ParticleShape
{
public:
	
	ofxBvh *bvh;
	
	vector<Tracker*> tracker;
	
	//vector<Particle> particles;
	//ofVboMesh billboards;
	//int particle_index;
	
	void setup(ofxBvh &o ,  int actorIndex)
	{
		bvh = &o;
		
		for (int i = 1; i < o.getNumJoints(); i++)
		{
			if (bvh->getJoint(i)->getName().find("Chest") == string::npos)
			{
				Tracker *t = new Tracker;
				t->setup(bvh->getJoint(i), bvh->getJoint(0),actorIndex);
				tracker.push_back(t);
			}
		}
		
		/*particle_index = 0;
		particles.resize(5000);
		billboards.getVertices().resize(5000);
		billboards.getColors().resize(5000);
		billboards.getNormals().resize(5000,0);
		billboards.setUsage( GL_DYNAMIC_DRAW );
		billboards.setMode(OF_PRIMITIVE_POINTS);
	
		for (int i = 0; i < particles.size(); i++)
		{
			Particle &p = particles[i];
			p.pos.set(0, 0, 0);
			p.vel.set(0, 0, 0);
			billboards.setColor(i,ofFloatColor(1,1,1,1));
		}*/
	}
	
	void update()
	{
		bvh->update();
		
		/*for (int i = 0; i < particles.size(); i++)
		{
			Particle &p = particles[i];
			p.force.set(0, 0, 0);
		}
		*/
		if (bvh->isFrameNew())
		{
			for (int i = 0; i < tracker.size(); i++)
			{
				// update force
				tracker[i]->update();//particles);
				
				//const ofVec3f &p = tracker[i]->joint->getPosition();
				//
				//// emit 10 particle every frame
				//for (int i = 0; i < 10; i++)
				//{
				//	particles[particle_index].pos.set(p);
				//	 
				//	particle_index++;
				//	if (particle_index > particles.size()-1)
				//		particle_index = 0;
				//}
			}
		}
		
		// update particle position
		/*for (int i = 0; i < particles.size(); i++)
		{
			Particle &p = particles[i];
			
			p.force.y += -0.1;
			p.vel *= 0.98;
			
			p.vel += p.force * 0.9;
			p.pos += p.vel * 0.9;
			
			if (p.pos.y <= 0)
			{
				p.pos.y = 0;
				p.vel *= 0.95;
			}
			billboards.getVertices()[i] = p.pos;
		}*/
	}
	
	void draw()
	{
		 bvh->draw();

		for (int i = 0; i < tracker.size(); i++)
		{
			tracker[i]->draw();
		}
		/*billboards.draw();
		//ofSetColor(255, 15);
		//glBegin(GL_POINTS);
		//for (int i = 0; i < particles.size(); i++)
		//{
		//	Particle &p = particles[i];
		//	glVertex3fv(p.pos.getPtr());
		//}
		//glEnd();
		*/
	}
};

const float trackDuration = 64.28;

const size_t NUM_ACTOR = 3;
vector<ParticleShape> particle_shapes;
vector<ofxBvh> bvh;

ofSoundPlayer player;

ofVec3f center;

//--------------------------------------------------------------
void testApp::setup()
{
    ofHideCursor();
	glow.allocate(ofGetWidth(),ofGetHeight());
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
	ofBackground(0);
	
	bvh.resize(NUM_ACTOR);
	particle_shapes.resize(NUM_ACTOR);
	
	// You have to get motion and sound data from http://www.perfume-global.com
	
	bvh[0].load("bvhfiles/aachan.bvh");
	bvh[1].load("bvhfiles/kashiyuka.bvh");
	bvh[2].load("bvhfiles/nocchi.bvh");
	
	for (int i = 0; i < NUM_ACTOR; i++)
	{
		bvh[i].setFrame(1);
		particle_shapes[i].setup(bvh[i],i);
	}
	
	player.loadSound("Perfume_globalsite_sound.wav");
	player.setLoop(OF_LOOP_NORMAL);
	player.play();
}

//--------------------------------------------------------------
void testApp::update()
{
	float t = (player.getPosition() * trackDuration);
	
	ofVec3f avg;
	
	for (int i = 0; i < NUM_ACTOR; i++)
	{
		ofxBvh *o = particle_shapes[i].bvh;
		
		o->setPosition(t / o->getDuration());
		particle_shapes[i].update();
		
		avg += o->getJoint(0)->getPosition();
	}
	
	avg /= 3;
	
	center += (avg - center) * 0.1;
}

//--------------------------------------------------------------
void testApp::draw()
{
	glow.setRadius(14);

	glow.begin();
	ofClear(0,0.1);

	

	glDisable(GL_DEPTH_TEST);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	// smooth particle
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	
	static GLfloat distance[] = {0.0, 0.0, 1.0};
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distance);
	glPointSize(1500);
	
	glLineWidth(2);
	
	cam.begin();
	
	/*ofPushMatrix();
	ofTranslate(0,50,0);
	ofDrawGrid(500, 10, false, false, true, false);
	ofPopMatrix();
	ofPopStyle();*/

	ofRotateY(ofGetElapsedTimef() * 10);
	ofTranslate(-center);
	
	for (int i = 0; i < NUM_ACTOR; i++)
	{
		
		particle_shapes[i].draw();
	}
	ofDisableBlendMode();

	cam.end();
	glow.end();
	glow.update();
	glow.draw(0,ofGetHeight(),ofGetWidth(),-ofGetHeight());
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	if (player.getSpeed() > 0)
		player.setSpeed(0);
	else
		player.setSpeed(1);
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}