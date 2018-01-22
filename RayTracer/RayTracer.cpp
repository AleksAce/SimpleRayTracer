#include "RayTracer.h"

static void InitGlew()
{

	GLenum err = glewInit();
	if (err != GLEW_OK)
		exit(1);
}
GLuint LoadTextures(char *file_name)
{
	int ImageX,
		ImageY,
		ImageComponents;

	GLubyte *ImageData = stbi_load(file_name, &ImageX, &ImageY, &ImageComponents, 4);
	//Get the Texture
	GLuint Texture;
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ImageX, ImageY, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageData);

	//for not using border colors GL_CLAMP uses border colors
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(ImageData);

	return Texture;
}
void BindTexture(GLenum TextureUnit, GLuint Texture)
{
	glActiveTexture(GL_TEXTURE0 + TextureUnit);
	glBindTexture(GL_TEXTURE_2D, Texture);

}
void releaseTextures(GLuint *Texture, GLuint *Sampler)
{
	glDeleteTextures(1, Texture);
	glDeleteSamplers(1, Sampler);

}



GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}


	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

GLuint CreateTexture(int width, int height, void *pixels = NULL)
{
	GLuint TextureID;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,pixels);

	glBindTexture(GL_TEXTURE_2D, 0);
	return TextureID;
}
static void SetupQuad()
{
	
	glm::vec3 vertices[4] =
	{
		glm::vec3(-1.0f,-1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f)
	};


	glEnableVertexAttribArray(0);

	glBufferData(GL_ARRAY_BUFFER, //full buffer size
		sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
}
inline float CheckPlaneIntersection(glm::vec3 RayStart, glm::vec3 RayDirection, object_plane Plane)
{
	//NOTE: This equation comes of the RayOrigin + distance*RayDireciton formula;
	// and the Plane Formula: N dot P + d = 0;
	// if we solve for distance; P = RayOrigin + distance*RayDireciton;
	// N dot (RayOrigin + distance*RayDireciton) + d = 0;
	// 

	//Note Prevent division by zero
	float d = dot(Plane.Normal, RayDirection);
	if (d < -0.00001 || d > 0.00001)
	{
		return 	(-Plane.Origin - glm::dot(Plane.Normal, RayStart)) / d;
	}
	return FLT_MAX;
}
inline float CheckSphereIntersection(glm::vec3 RayStart, glm::vec3 RayDirection, object_sphere Sphere)
{
	//Quadratic formula (2 intersections of the sphere happen)
	//FROM WIKI
	// posX - pos0 ^2 + posY - pos0 ^2 + posZ - pos0^2 - radius^2 = 0;
	// its actually the dot product of the vector by itself
	// (pos dot pos) - radius^2 =0;
	// we need to calculate the exact position with the 
	// ray formula pos = ROr + distance*Rdir;
	// (ROr + dist*Rdir) dot (ROr + dist*Rdir)
	// ROr dot ROr + 2dist(Rdir dot ROr) + dist^2*(Rdir dot RDir) - radius^2
	// distance^2*(Rdir dot Rdir) +  distance*(2*(Rdir dot ROr)) + (ROr dot ROr - radius)= 0
	
	//MAP the ray relative to the sphere the PosX - pos0.x PosY - pos0.y
	RayStart = RayStart - Sphere.Position;

	float a = dot(RayDirection,RayDirection);
	float b = 2.0f*glm::dot(RayDirection,RayStart);
	float c = dot(RayStart,RayStart) - Sphere.Radius;
	float d = 2.0f*a;
	float root = sqrt(b*b - 4.0f*a*c);
	if (root > 0.00001f)
	{
		float pos0 = (-b + root) / d;
		float pos1 = (-b - root) / d;
		
		//pick the closer one
		float pos;
		if (pos0 > 0 && pos0 < pos1)
		{
			pos = pos0;
		}
		else
		{
			pos = pos1;
		}

		//check against division by zero
		if (d < -0.00001 || d > 0.00001)
		{
			return pos;
		}
	}
    //if division by 0 happened
	return FLT_MAX;
}
static glm::vec4 Trace(scene *Scene, glm::vec3 RayStart, glm::vec3 RayDirection)
{
	
	glm::vec4 Color = glm::vec4(0.0f);
	//or Attenuation
	glm::vec4 ColorWeakness = glm::vec4(1.0);
	float MinRayHitDistance = FLT_MAX;
	
	//Cast many rays
	for (int i = 0; i < 8; i++)
	{
		bool Hit = false;
		material Material = {};
		glm::vec3 NextRayStart;
		glm::vec3 NextRayDirection;

		//TRACE ALL THE PLANES
		for (int PlaneIndex = 0; PlaneIndex < Scene->PlaneCount; PlaneIndex++)
		{
			object_plane Plane = Scene->Planes[PlaneIndex];

			float PlaneHitDistance = CheckPlaneIntersection(RayStart, RayDirection, Plane);
			//NOTE: Check what is closer. If closer 
			if ((PlaneHitDistance > 0) && (PlaneHitDistance < MinRayHitDistance))
			{
				MinRayHitDistance = PlaneHitDistance;
				Material = Plane.Material;
				Hit = true;
				//we start where we hit
				NextRayStart =RayStart + PlaneHitDistance*RayDirection;
				//and trace from the normal
				NextRayDirection = Plane.Normal;
				
			}

		}
		//TRACE ALL THE SPHERES
		for (int SphereIndex = 0; SphereIndex < Scene->SpheresCount; SphereIndex++)
		{
			object_sphere Sphere = Scene->Sphere[SphereIndex];

			float SphereHitDistance = CheckSphereIntersection(RayStart, RayDirection, Sphere);
			//NOTE: Check what is closer. If closer 
			if ((SphereHitDistance > 0.0) && (SphereHitDistance < MinRayHitDistance))
			{
				MinRayHitDistance = SphereHitDistance;
				Material = Sphere.Material;
				Hit = true;
				NextRayStart = RayStart + SphereHitDistance*RayDirection;
				//from the center of the sphere to the Origin 
				NextRayDirection = glm::normalize(NextRayStart - Sphere.Position);
				
			}

		}	
		if (Hit)
		{
			
			Color += ColorWeakness * Material.Color;
			//Color += ColorWeakness * Scene->Materials[MaterialIndex].ReflectionColor;
			//TheColor gets weaker as we trace more.
			//ColorWeakness = ColorWeakness * Scene->Materials[MaterialIndex].ReflectionColor;
			//ColorWeakness = ColorWeakness * Scene->Materials[MaterialIndex].Color;
			//the reflection angle
			float reflection_value = glm::dot(-RayDirection, NextRayDirection);
			ColorWeakness = ColorWeakness * reflection_value*Material.ReflectionColor;

			RayStart = NextRayStart;
			//Specular lighting
			RayDirection = RayDirection - 2.0f*glm::dot(RayDirection, NextRayDirection)*NextRayDirection;
			glm::vec3 RandomRayDirection = glm::normalize(NextRayDirection + glm::vec3(rand()/(float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX));
			
			//turn this on if you want random raycasts
			RayDirection = glm::normalize(Lerp(RandomRayDirection, Material.specular_value, RayDirection));
			
		}
		else
		{
			//in this case the index is 0 and we add the color of the Sky .. the 0th color
			
			Color +=ColorWeakness * Scene->Materials[0].Color;
			break;
		}

	}
	
	return Color;
}
static void SetupObjects(material *Material, object_plane *Plane, object_sphere *Sphere, scene *Scene)
{
	
    Material[0].Color = glm::vec4(1.0, 1.0, 1.0, 0.8);
    //Material[1].Color = glm::vec4(0.5, 0.5, 0.5, 1.0);
    //Material[2].Color = glm::vec4(0.3, 0.4, 0.0, 1.0);
    //Material[3].Color = glm::vec4(0.0, 0.4, 0.5, 1.0);
    //Material[4].Color = glm::vec4(0.0, 0.0, 0.6, 1.0);
    //Material[5].Color = glm::vec4(0.3, 0.4, 0.8, 1.0);
    //Material[6].Color = glm::vec4(0.8, 0.8, 0.8, 1.0);
	Material[1].specular_value = 1.0;
	Material[2].specular_value = 0.0;
	Material[3].specular_value = 0.0;
	Material[4].specular_value = 0.0;
	Material[5].specular_value = 0.0;
	Material[6].specular_value = 0.0;
	Material[7].specular_value = 1.0;
	//Material[1].Color = glm::vec4(0.1, 0.1, 0.1, 1.0);
	Material[1].ReflectionColor = glm::vec4(0.3, 0.3, 0.3, 1.0);
	Material[2].ReflectionColor = glm::vec4(0.0, 0.25, 0.0, 1.0);
	//the Color value emits light
	//Material[2].Color = glm::vec4(0.0, 0.25, 0.0, 1.0);
	Material[3].ReflectionColor = glm::vec4(0.8, 0.0, 0.0, 1.0);
//	Material[3].Color = glm::vec4(0.8, 0.1, 0.1, 1.0);
	Material[4].ReflectionColor = glm::vec4(0.6, 0.3, 0.0, 1.0);
	Material[5].ReflectionColor = glm::vec4(0.0, 0.4, 0.3, 1.0);
	Material[6].ReflectionColor = glm::vec4(1.0, 0.0,0.1, 1.0);
	Material[7].ReflectionColor = glm::vec4(0.70, 0.70, 0.70, 1.0);
	
	Scene->Materials = Material;

	
	Plane[0].Material = Material[1];
	Plane[0].Normal = glm::vec3(0.0, 0.0, 1.0);
	Plane[0].Origin = 0.0;
	
	Scene->Planes = Plane;
	
	Sphere[0].Position = glm::vec3(0.0, 0.0, 0.0);
	Sphere[0].Radius = 1.0f;
	Sphere[0].Material = Material[2];
	Sphere[1].Position = glm::vec3(-2.0, 3.0, 1.0);
	Sphere[1].Radius = 1.0f;
	Sphere[1].Material = Material[3];
	Sphere[2].Position = glm::vec3(3.0, -1.0, 2.0);
	Sphere[2].Radius = 1.0f;
	Sphere[2].Material = Material[4];
	Sphere[3].Position = glm::vec3(0.0, -2.0, 2.0);
	Sphere[3].Radius = 1.0f;
	Sphere[3].Material = Material[5];
	Sphere[4].Position = glm::vec3(2.0, -4.0, 4.0);
	Sphere[4].Radius = 1.0f;
	Sphere[4].Material = Material[6];
	Sphere[5].Position = glm::vec3(-2.0, 2.0, 3.0);
	Sphere[5].Radius = 1.0f;
	Sphere[5].Material = Material[7];
	
	Scene->Sphere = Sphere;

}
static texture SetupTexture()
{
	texture Texture = {};
	Texture.Width = 1280;//WindowWidth;
	Texture.Height = 720;//WindowHeight;
	//Texture.Width = WindowWidth;
	//Texture.Height = WindowHeight;

	scene Scene = {};
	Scene.MaterialCount = 7;
	Scene.PlaneCount = 1;
	Scene.SpheresCount = 6;

	material Material[8] = {};
	object_plane Plane[1] = {};
	object_sphere Sphere[6] = {};
	
	SetupObjects(Material, Plane, Sphere, &Scene);
	
	
	
	glm::vec3 Origin = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 UPVector = glm::vec3(0.0, 0.0, 1.0);
	//glm::vec3 camera_position = glm::vec3(3.0f,10.0f,3.0f);
	//glm::vec3 camera_direction = glm::vec3(0.1f, -0.2f, 1.0f);
	glm::vec3 camera_position = glm::vec3(0.0f, 10.0f, 1.0f);
	glm::vec3 camera_direction = glm::vec3(0.1f, -0.2f, 1.0f);
	//Get Camera position axis, we start with the Z axis and get the rest of the axis from that one
	glm::vec3 CameraAxisZ = glm::normalize(camera_position - Origin);
	glm::vec3 CameraAxisX = glm::normalize(glm::cross(CameraAxisZ, UPVector));
	glm::vec3 CameraAxisY = glm::normalize(glm::cross(CameraAxisX, CameraAxisZ));

	//Find the scene center so we can start tracing from it
	//its the Distance we want on the Z axis


	//SCENE DISTANCE RELATIVE TO THE CAMERA. For the camera_position CameraAxis is 1,1,1
	glm::vec3 SceneCenter = camera_position - camera_direction.x*CameraAxisX - camera_direction.y*CameraAxisY - camera_direction.z*CameraAxisZ;


	float SceneWidth = 1.0f;
	float SceneHeight = SceneWidth* ((float)Texture.Height/(float)Texture.Width);

	float HalfSceneWidth = 0.5f*SceneWidth;
	float HalfSceneHeight = 0.5f*SceneHeight;

	Texture.PixelData = malloc(sizeof(GLuint) * Texture.Width * Texture.Height);
	GLubyte *Pixel = (GLubyte *)Texture.PixelData;

	printf("Start \n");

	for (int TextureY = 0; TextureY < Texture.Height; TextureY++)
	{
		if (TextureY % 100 == 0)
		{
			int percent = (int)((TextureY*100 / Texture.Height));
			printf("%d  Done \n", percent);
		}
		//NOTE: These are from -1 to 1
		float Y = 2.0*((float)TextureY / (float)Texture.Height) - 1.0f;
		for (int TextureX = 0; TextureX < Texture.Width; TextureX++)
		{
			
			float X = 2.0*((float)TextureX / (float)Texture.Width) -1.0f;

			X = X + (rand() / RAND_MAX)*(0.5/Texture.Width);
			Y = Y + (rand() / RAND_MAX)*(0.5 / Texture.Height);

			//NOTE: Multiply by the Camera Axis
			//NOTE: SceneWidth/height controls the aspect ratio
			//THIS IS THE SCENE POSITION RELATIVE TO THE CAMERA
			glm::vec3 ScenePos = SceneCenter + HalfSceneWidth*CameraAxisX*X + HalfSceneHeight*CameraAxisY*Y;

			//TRACE
			glm::vec3 RayStart = camera_position;
			glm::vec3 RayDirection = glm::normalize(ScenePos - camera_position);
			
			glm::vec4 color;
			float num_rays = 8.0f;
			float value = 1.0f / num_rays;
			for (int i = 0; i < num_rays; i++)
			{
				color += value*Trace(&Scene, RayStart, RayDirection);
			}

			//Transfor the color to ubyte and gammacorrect it
			color = glm::vec4(LinearTosRGB(color.r), LinearTosRGB(color.g), LinearTosRGB(color.b),255.0f);

			//convert to UBYTE
			*Pixel++ = (color.r * 0xFF);
			*Pixel++ = (color.g * 0xFF);
			*Pixel++ = (color.b * 0xFF);
			*Pixel++ = (color.a * 0xFF);
			
		}
		
	}
	 
		
	printf("100 Done \n");

	 Texture.TextureHandle = CreateTexture(Texture.Width, Texture.Height, Texture.PixelData);
	
	//Delete the memory
	delete[] Texture.PixelData;

	return Texture;
}

int main(int argc, char* args[])
{
	//The window we'll be rendering to
	SDL_Window* window = NULL;


	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Cannot initialize SDL video!\n");
		exit(1);
	}

	//Create window
	window = SDL_CreateWindow("RayTracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WindowWidth, WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	SDL_GLContext gContext = NULL;

	//Create context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	gContext = SDL_GL_CreateContext(window);
	if (gContext == NULL)
	{
		printf("%s\n", SDL_GetError());
	}

	InitGlew();

	//Setup opengl
	glViewport(0, 0, WindowWidth, WindowHeight);
	glEnable(GL_TEXTURE_2D);
	
	GLuint programID;
	GLuint VAO, VBO_position;
	
	programID = LoadShaders("simpleShader.vert", "simpleShader.frag");
	glUseProgram(programID);

	SDL_GL_SetSwapInterval(0);

	//VAOS and VBOS
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO_position);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	
	texture Texture1 = SetupTexture();
    glUniform1i(glGetUniformLocation(programID, "gSampler"),0);
	
	const Uint8* state =  SDL_GetKeyboardState(NULL);
	while (1)
	{
		state = SDL_GetKeyboardState(NULL);
		SDL_PumpEvents();
		if (state[SDL_SCANCODE_ESCAPE])
		{
			break;
		}

		//Draw the scene
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0, 1.0, 0.0, 1.0);
		
		SetupQuad();
		//Draw the FULLSCREEN quad
		BindTexture(0, Texture1.TextureHandle);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
		
		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
	return 0;
}