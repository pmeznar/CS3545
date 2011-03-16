/*
===========================================================================
File:		renderer_model_ASE.c
Author: 	Clinton Freeman
Created on: Sep 30, 2010
Notes:		Please remember that this is, for the moment, basically
			just ripped directly out of my project, and so you will
			need to do some re-working to fit it into your project.
===========================================================================
*/

#include "headers/SDL/SDL_opengl.h"

#include "headers/common.h"
#include "headers/files.h"
#include "headers/mathlib.h"

#include "headers/renderer_materials.h"
#include "headers/renderer_models.h"

static void loadASE_parseTokens(char **tokens, int numTokens, eboolean collidable);
static void loadASE_generateList(int index);

/*
===========================================================================
Data Structures
===========================================================================
*/

typedef struct
{
	char	name[MAX_NAMELENGTH], class[MAX_NAMELENGTH];
	int		subNo;
	float	amount;
	char	bitmap[MAX_FILEPATH];
	char	type[MAX_NAMELENGTH];
	float	uvw_uOffset, uvw_vOffset, uvw_uTiling, uvw_vTiling,
			uvw_angle, uvw_blur, uvw_blurOffset, uvw_noiseAmt,
			uvw_noiseSize, uvw_noiseLevel, uvw_noisePhase;
	char	bitmapFilter[MAX_NAMELENGTH];
}
ase_mapDiffuse_t;

typedef struct
{
	int		id, globalID;
	char	name[MAX_NAMELENGTH], class[MAX_NAMELENGTH];
	vec3_t	ambient, diffuse, specular;
	float	shine, shineStrength, transparency, wireSize;
	char	shading[MAX_NAMELENGTH];
	float	xpFalloff, selfIllum;
	char	falloff[MAX_NAMELENGTH], xpType[MAX_NAMELENGTH];

	ase_mapDiffuse_t diffuseMap;
}
ase_material_t;

typedef struct
{
	int				materialCount;
	ase_material_t	*list;
}
ase_materialList_t;

typedef struct
{
	int 	faceID;
	int 	A, B, C, AB, BC, CA;
	int 	smoothingGroup, materialID;
	vec3_t	normal;
}
ase_mesh_face_t;

typedef struct
{
	int	tfaceID;
	int a, b, c;
}
ase_mesh_tface_t;

typedef struct
{
	int 	vertexID;
	vec3_t	coords;
	vec3_t	normal;
}
ase_mesh_vertex_t;

typedef struct
{
	int 	vertexID;
	vec3_t	coords;
}
ase_mesh_tvertex_t;

typedef struct
{
	int 	numVertex, numFaces;
	int 	numTVertex, numTVFaces;

	ase_mesh_vertex_t	*vertexList;
	ase_mesh_tvertex_t	*tvertList;
	ase_mesh_face_t		*faceList;
	ase_mesh_tface_t	*tfaceList;
}
ase_mesh_t;

typedef struct
{
	char 		name[MAX_NAMELENGTH];
	ase_mesh_t 	mesh;
	int			materialRef;
}
ase_geomObject_t;

typedef struct
{
	int 				numObjects;
	int					glListID;
	ase_geomObject_t	*objects;
	ase_materialList_t	materials;
}
ase_model_t;

//Debugging
static void loadASE_printDiffuse(ase_mapDiffuse_t *diffuse);
static void loadASE_printMaterial(ase_material_t *material);
static void loadASE_printMaterialList(ase_materialList_t *materialList);
static void loadASE_printFace(ase_mesh_face_t *face);
static void loadASE_printTFace(ase_mesh_tface_t *tface);
static void loadASE_printVertex(ase_mesh_vertex_t *vertex);
static void loadASE_printTVertex(ase_mesh_tvertex_t *tvert);
static void loadASE_printMesh(ase_mesh_t *mesh);
static void loadASE_printGeomObject(ase_geomObject_t *geomObject);
static void loadASE_printModel(ase_model_t *model);

static ase_model_t 	modelStack[MAX_MODELS];
static int 			modelPtr = 0;

/*
 * renderer_model_loadASE
 */
void renderer_model_loadASE(char *name, eboolean collidable)
{
	FILE 			*file;
	unsigned int	fileSize;
	char 			*fileBuffer, **tokens;
	unsigned int	numTokens, blah;

	//Attempt to load the specified file
	file = fopen(name, "r");

	if(file == NULL)
	{
		printf("Loading ASE: %s, failed. Null file pointer.\n", name);
		return;
	}

	//Determine the size of the file
	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	rewind(file);

	//If successful, load the file's contents into a buffer
	fileBuffer = (char *)malloc(sizeof(char) * fileSize);
	blah = fread(fileBuffer, sizeof(char), fileSize+1, file);
	fileBuffer[blah] = '\0';

	fclose(file);

	//Break the file into a new array of tokens
	numTokens = files_tokenizeStr(fileBuffer, " \t\n\r\0", &tokens);
	free(fileBuffer);

	loadASE_parseTokens(tokens, numTokens, collidable);
}

/*
 * loadASE_parseTokens
 */
static void loadASE_parseTokens(char **tokens, int numTokens, eboolean collidable)
{
	int i, j, curMatID, curObj, curFNormal, curVNormal;
	ase_model_t	*model;
	ase_mesh_vertex_t *vertexList;
	ase_mesh_face_t *faceList;
	vec3_t tri[3];

	model = &(modelStack[modelPtr]);

	for(i = 0; i < numTokens; i++)
	{
		if(!strcmp(tokens[i], "*MATERIAL_COUNT"))
		{
			model->materials.materialCount = atoi(tokens[++i]);

			//Allocate enough space for the given number of materials.
			model->materials.list = (ase_material_t *)malloc(sizeof(ase_material_t) * model->materials.materialCount);
		}
		else if(!strcmp(tokens[i], "*MATERIAL"))
		{
			curMatID = atoi(tokens[++i]);
			model->materials.list[curMatID].id = curMatID;
		}
		else if(!strcmp(tokens[i], "*MATERIAL_NAME"))
			strcpy(model->materials.list[curMatID].name, tokens[++i]);
		else if(!strcmp(tokens[i], "*MATERIAL_CLASS"))
			strcpy(model->materials.list[curMatID].class, tokens[++i]);
		else if(!strcmp(tokens[i], "*MATERIAL_AMBIENT"))
		{
			model->materials.list[curMatID].ambient[_X] = atoi(tokens[++i]);
			model->materials.list[curMatID].ambient[_Y] = atoi(tokens[++i]);
			model->materials.list[curMatID].ambient[_Z] = atoi(tokens[++i]);
		}
		else if(!strcmp(tokens[i], "*MATERIAL_DIFFUSE"))
		{
			model->materials.list[curMatID].diffuse[_X] = atoi(tokens[++i]);
			model->materials.list[curMatID].diffuse[_Y] = atoi(tokens[++i]);
			model->materials.list[curMatID].diffuse[_Z] = atoi(tokens[++i]);
		}
		else if(!strcmp(tokens[i], "*MATERIAL_SPECULAR"))
		{
			model->materials.list[curMatID].specular[_X] = atoi(tokens[++i]);
			model->materials.list[curMatID].specular[_Y] = atoi(tokens[++i]);
			model->materials.list[curMatID].specular[_Z] = atoi(tokens[++i]);
		}
		else if(!strcmp(tokens[i], "*MATERIAL_SHINE"))
			model->materials.list[curMatID].shine = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*MATERIAL_SHINESTRENGTH"))
			model->materials.list[curMatID].shineStrength = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*MATERIAL_TRANSPARENCY"))
			model->materials.list[curMatID].transparency = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*MATERIAL_WIRESIZE"))
			model->materials.list[curMatID].wireSize = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*MATERIAL_SHADING"))
			strcpy(model->materials.list[curMatID].shading, tokens[++i]);
		else if(!strcmp(tokens[i], "*MATERIAL_XP_FALLOFF"))
			model->materials.list[curMatID].xpFalloff = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*MATERIAL_SELFILLUM"))
			model->materials.list[curMatID].selfIllum = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*MATERIAL_FALLOFF"))
			strcpy(model->materials.list[curMatID].falloff, tokens[++i]);
		else if(!strcmp(tokens[i], "*MATERIAL_XP_TYPE"))
			strcpy(model->materials.list[curMatID].xpType, tokens[++i]);

		else if(!strcmp(tokens[i], "*MAP_NAME"))
			strcpy(model->materials.list[curMatID].diffuseMap.name, tokens[++i]);
		else if(!strcmp(tokens[i], "*MAP_CLASS"))
			strcpy(model->materials.list[curMatID].diffuseMap.class, tokens[++i]);
		else if(!strcmp(tokens[i], "*MAP_SUBNO"))
			model->materials.list[curMatID].diffuseMap.subNo = atoi(tokens[++i]);
		else if(!strcmp(tokens[i], "*MAP_AMOUNT"))
			model->materials.list[curMatID].diffuseMap.amount = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*BITMAP"))
			strcpy(model->materials.list[curMatID].diffuseMap.bitmap, tokens[++i]);
		else if(!strcmp(tokens[i], "*MAP_TYPE"))
			strcpy(model->materials.list[curMatID].diffuseMap.type, tokens[++i]);
		else if(!strcmp(tokens[i], "*UVW_U_OFFSET"))
			model->materials.list[curMatID].diffuseMap.uvw_uOffset = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*UVW_V_OFFSET"))
			model->materials.list[curMatID].diffuseMap.uvw_vOffset = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*UVW_U_TILING"))
			model->materials.list[curMatID].diffuseMap.uvw_uTiling = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*UVW_V_TILING"))
			model->materials.list[curMatID].diffuseMap.uvw_vTiling = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*UVW_ANGLE"))
			model->materials.list[curMatID].diffuseMap.uvw_angle = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*UVW_BLUR"))
			model->materials.list[curMatID].diffuseMap.uvw_blur = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*UVW_BLUR_OFFSET"))
			model->materials.list[curMatID].diffuseMap.uvw_blurOffset = atof(tokens[++i]);
		//TYPO in the exporter!!!!!!
		else if(!strcmp(tokens[i], "*UVW_NOUSE_AMT"))
			model->materials.list[curMatID].diffuseMap.uvw_noiseAmt = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*UVW_NOISE_SIZE"))
			model->materials.list[curMatID].diffuseMap.uvw_noiseSize = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*UVW_NOISE_LEVEL"))
			model->materials.list[curMatID].diffuseMap.uvw_noiseLevel = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*UVW_NOISE_PHASE"))
			model->materials.list[curMatID].diffuseMap.uvw_noisePhase = atof(tokens[++i]);
		else if(!strcmp(tokens[i], "*BITMAP_FILTER"))
			strcpy(model->materials.list[curMatID].diffuseMap.bitmapFilter, tokens[++i]);

		else if(!strcmp(tokens[i], "*GEOMOBJECT"))
		{
			model->numObjects++;
			model->objects = (ase_geomObject_t *)realloc(model->objects, sizeof(ase_geomObject_t) * model->numObjects);
			curObj = model->numObjects - 1;
		}
		else if(!strcmp(tokens[i], "*NODE_NAME"))
			strcpy(model->objects[curObj].name, tokens[++i]);
		else if(!strcmp(tokens[i], "*MESH_NUMVERTEX"))
		{
			model->objects[curObj].mesh.numVertex = atoi(tokens[++i]);
			model->objects[curObj].mesh.vertexList =
					(ase_mesh_vertex_t *)malloc(sizeof(ase_mesh_vertex_t) * model->objects[curObj].mesh.numVertex);
		}
		else if(!strcmp(tokens[i], "*MESH_NUMFACES"))
		{
			model->objects[curObj].mesh.numFaces = atoi(tokens[++i]);
			model->objects[curObj].mesh.faceList =
					(ase_mesh_face_t *)malloc(sizeof(ase_mesh_face_t) * model->objects[curObj].mesh.numFaces);
		}
		else if(!strcmp(tokens[i], "*MESH_VERTEX_LIST"))
		{
			//Skip *MESH_VERTEX_LIST and {
			i += 2;

			for(j = 0; j < model->objects[curObj].mesh.numVertex; j++)
			{
				//Skip *MESH_VERTEX
				i++;

				model->objects[curObj].mesh.vertexList[j].vertexID   = atoi(tokens[i++]);
				model->objects[curObj].mesh.vertexList[j].coords[_X] = atof(tokens[i++]);
				model->objects[curObj].mesh.vertexList[j].coords[_Y] = atof(tokens[i++]);
				model->objects[curObj].mesh.vertexList[j].coords[_Z] = atof(tokens[i++]);
			}
		}
		else if(!strcmp(tokens[i], "*MESH_FACE_LIST"))
		{
			//Skip *MESH_FACE_LIST and {
			i += 2;

			for(j = 0; j < model->objects[curObj].mesh.numFaces; j++)
			{
				//Skip *MESH_FACE and #:
				i += 2;

				model->objects[curObj].mesh.faceList[j].faceID = j;

				//Skip A:
				i++; model->objects[curObj].mesh.faceList[j].A = atoi(tokens[i++]);
				//Skip B:
				i++; model->objects[curObj].mesh.faceList[j].B = atoi(tokens[i++]);
				//Skip C:
				i++; model->objects[curObj].mesh.faceList[j].C = atoi(tokens[i++]);
				//Skip AB:
				i++; model->objects[curObj].mesh.faceList[j].AB = atoi(tokens[i++]);
				//Skip BC:
				i++; model->objects[curObj].mesh.faceList[j].BC = atoi(tokens[i++]);
				//Skip CA:
				i++; model->objects[curObj].mesh.faceList[j].CA = atoi(tokens[i++]);
				//Skip *MESH_SMOOTHING
				//Its possible to not have a smoothing group number, in which case we'll accidentally gobble
				//up too many tokens
				i++;

				//If the next token IS NOT *MESH_MTLID
				if(strcmp(tokens[i], "*MESH_MTLID"))
					//Then grab
					model->objects[curObj].mesh.faceList[j].smoothingGroup = atoi(tokens[i++]);

				//Skip *MESH_MTLID
				i++; model->objects[curObj].mesh.faceList[j].materialID = atoi(tokens[i++]);
			}
		}
		else if(!strcmp(tokens[i], "*MESH_NUMTVERTEX"))
		{
			model->objects[curObj].mesh.numTVertex = atoi(tokens[++i]);
			model->objects[curObj].mesh.tvertList =
					(ase_mesh_tvertex_t *)malloc(sizeof(ase_mesh_tvertex_t) * model->objects[curObj].mesh.numTVertex);
		}
		else if(!strcmp(tokens[i], "*MESH_TVERTLIST"))
		{
			//Skip *MESH_TVERTLIST and {
			i += 2;

			for(j = 0; j < model->objects[curObj].mesh.numTVertex; j++)
			{
				//Skip *MESH_TVERTEX
				i++;

				model->objects[curObj].mesh.tvertList[j].vertexID   = atoi(tokens[i++]);
				model->objects[curObj].mesh.tvertList[j].coords[_X] = atof(tokens[i++]);
				model->objects[curObj].mesh.tvertList[j].coords[_Y] = atof(tokens[i++]);
				model->objects[curObj].mesh.tvertList[j].coords[_Z] = atof(tokens[i++]);
			}
		}
		else if(!strcmp(tokens[i], "*MESH_NUMTVFACES"))
		{
			model->objects[curObj].mesh.numTVFaces = atoi(tokens[++i]);
			model->objects[curObj].mesh.tfaceList =
					(ase_mesh_tface_t *)malloc(sizeof(ase_mesh_tface_t) * model->objects[curObj].mesh.numTVFaces);
		}
		else if(!strcmp(tokens[i], "*MESH_TFACELIST"))
		{
			//Skip *MESH_TFACELIST and {
			i += 2;

			for(j = 0; j < model->objects[curObj].mesh.numTVFaces; j++)
			{
				//Skip *MESH_TFACE
				i++;

				model->objects[curObj].mesh.tfaceList[j].tfaceID = atoi(tokens[i++]);
				model->objects[curObj].mesh.tfaceList[j].a = atoi(tokens[i++]);
				model->objects[curObj].mesh.tfaceList[j].b = atoi(tokens[i++]);
				model->objects[curObj].mesh.tfaceList[j].c = atoi(tokens[i++]);
			}
		}
		else if(!strcmp(tokens[i], "*MESH_FACENORMAL"))
		{
			curFNormal = atoi(tokens[++i]);
			model->objects[curObj].mesh.faceList[curFNormal].normal[_X] = atof(tokens[++i]);
			model->objects[curObj].mesh.faceList[curFNormal].normal[_Y] = atof(tokens[++i]);
			model->objects[curObj].mesh.faceList[curFNormal].normal[_Z] = atof(tokens[++i]);
		}

		else if(!strcmp(tokens[i], "*MESH_VERTEXNORMAL"))
		{
			curVNormal = atoi(tokens[++i]);
			model->objects[curObj].mesh.vertexList[curVNormal].normal[_X] = atof(tokens[++i]);
			model->objects[curObj].mesh.vertexList[curVNormal].normal[_Y] = atof(tokens[++i]);
			model->objects[curObj].mesh.vertexList[curVNormal].normal[_Z] = atof(tokens[++i]);
		}
		else if(!strcmp(tokens[i], "*MATERIAL_REF"))
			model->objects[curObj].materialRef = atoi(tokens[++i]);
	}

	//Create OpenGL textures from our materials, and store the global material indices
	for(i = 0; i < model->materials.materialCount; i++)
	{
		model->materials.list[i].globalID = renderer_img_createMaterial(model->materials.list[i].diffuseMap.bitmap,
				model->materials.list[i].ambient, model->materials.list[i].diffuse, model->materials.list[i].specular,
				model->materials.list[i].shine, model->materials.list[i].shineStrength, model->materials.list[i].transparency);
	}

	//Correct the mesh's references to point to the global material
	for(i = 0; i < model->numObjects; i++)
		model->objects[i].materialRef = model->materials.list[model->objects[i].materialRef].globalID;

	/*
	//Potentially add triangles to collision list
	if(collidable)
	{
		for(i = 0; i < model->numObjects; i++)
		{
			world_allocCollisionTris(model->objects[i].mesh.numFaces);

			vertexList  = model->objects[i].mesh.vertexList;
			faceList    = model->objects[i].mesh.faceList;

			for(j = 0; j < model->objects[i].mesh.numFaces; j++)
			{
				VectorCopy(vertexList[faceList[j].A].coords, tri[0]);
				VectorCopy(vertexList[faceList[j].B].coords, tri[1]);
				VectorCopy(vertexList[faceList[j].C].coords, tri[2]);

				world_addCollisionTri(tri);
			}
		}
	}
	*/

	//Generate a display list for drawing
	model->glListID = glGenLists(1);
	glNewList(model->glListID, GL_COMPILE);
		loadASE_generateList(modelPtr);
	glEndList();

	modelPtr++;
}

/*
 * loadASE_generateList
 */
static void loadASE_generateList(int index)
{
	int i, j;
	ase_model_t 		*model;
	ase_mesh_vertex_t 	*vertexList;
	ase_mesh_face_t 	*faceList;
	ase_mesh_tface_t 	*tfaceList;
	ase_mesh_tvertex_t 	*tvertList;

	model = &(modelStack[index]);

	for(i = 0; i < model->numObjects; i++)
	{
		vertexList  = model->objects[i].mesh.vertexList;
		tvertList   = model->objects[i].mesh.tvertList;
		faceList    = model->objects[i].mesh.faceList;
		tfaceList   = model->objects[i].mesh.tfaceList;

		glBindTexture(GL_TEXTURE_2D,
				renderer_img_getMatGLID(model->objects[i].materialRef));

		for(j = 0; j < model->objects[i].mesh.numFaces; j++)
		{
			glBegin(GL_POLYGON);
				glNormal3fv(faceList[j].normal);

				//glNormal3fv(vertexList[faceList[j].A].normal);
				glTexCoord3fv(tvertList[tfaceList[j].a].coords);
				glVertex3fv(vertexList[faceList[j].A].coords);

				//glNormal3fv(vertexList[faceList[j].B].normal);
				glTexCoord3fv(tvertList[tfaceList[j].b].coords);
				glVertex3fv(vertexList[faceList[j].B].coords);

				//glNormal3fv(vertexList[faceList[j].C].normal);
				glTexCoord3fv(tvertList[tfaceList[j].c].coords);
				glVertex3fv(vertexList[faceList[j].C].coords);
			glEnd();
		}
	}
}

/*
 * renderer_model_drawASE
 */
void renderer_model_drawASE(int index)
{
	glCallList(modelStack[index].glListID);
}

/*
===========================================================================
Debugging
===========================================================================
*/

/*
 * Function: loadASE_printDiffuse
 * Description:
 */
static void loadASE_printDiffuse(ase_mapDiffuse_t *diffuse)
{
	printf("Name: %s\n", 	diffuse->name);
	printf("Class: %s\n", 	diffuse->class);
	printf("SubNo: %d\n", 	diffuse->subNo);
	printf("Amount: %f\n", 	diffuse->amount);
	printf("Bitmap: %s\n", 	diffuse->bitmap);
	printf("Type: %s\n", 	diffuse->type);

	printf("UVW U Offset: %f, UVW V Offset: %f\n", diffuse->uvw_uOffset, diffuse->uvw_vOffset);
	printf("UVW Angle: %f, UVW Blur: %f, UVW Blur Offset: %f\n", diffuse->uvw_angle,
			diffuse->uvw_blur, diffuse->uvw_blurOffset);
	printf("UVW Noise Amount: %f, UVW Noise Level: %f, UVW Noise Phase: %f\n", diffuse->uvw_noiseAmt,
			diffuse->uvw_noiseLevel, diffuse->uvw_noisePhase);
	printf("Bitmap Filter: %s\n", diffuse->bitmapFilter);
}

/*
 * Function: loadASE_printMaterial
 * Description:
 */
static void loadASE_printMaterial(ase_material_t *material)
{
	printf("Material ID: %d\n", 		material->id);
	printf("Global ID: %d\n", 			material->globalID);
	printf("Name: %s\n", 				material->name);
	printf("Class: %s\n", 				material->class);

	printf("Ambient: R = %f, G = %f, B = %f\n", material->ambient[_R],
			material->ambient[_G], material->ambient[_B]);
	printf("Diffuse: R = %f, G = %f, B = %f\n", material->diffuse[_R],
			material->diffuse[_G], material->diffuse[_B]);
	printf("Specular: R = %f, G = %f, B = %f\n", material->specular[_R],
			material->specular[_G], material->specular[_B]);

	printf("Shine: %f\n", 				material->shine);
	printf("Shine Strength: %f\n", 		material->shineStrength);
	printf("Transparency: %f\n", 		material->transparency);
	printf("Wire Size: %f\n", 			material->wireSize);
	printf("Shading: %s\n", 			material->shading);
	printf("XP Falloff: %f\n",  		material->xpFalloff);
	printf("Self-Illumination: %f\n", 	material->selfIllum);
	printf("Falloff: %s\n", 			material->falloff);
	printf("XP Type: %s\n", 			material->xpType);

	printf("=======================================================\n");

	loadASE_printDiffuse(&(material->diffuseMap));
}

/*
 * Function: loadASE_printMaterialList
 * Description:
 */
static void loadASE_printMaterialList(ase_materialList_t *materialList)
{
	int i;

	printf("Material Count: %d\n", materialList->materialCount);

	for(i = 0; i < materialList->materialCount; i++)
	{
		printf("=======================================================\n");
		loadASE_printMaterial(&(materialList->list[i]));
	}
}

/*
 * Function: loadASE_printFace
 * Description:
 */
static void loadASE_printFace(ase_mesh_face_t *face)
{
	printf("Face ID: %d\n", 		face->faceID);

	printf("A: %d, B: %d, C: %d, AB: %d, BC: %d, CA: %d\n", face->A, face->B, face->C,
			face->AB, face->BC, face->CA);

	printf("Smoothing Group: %d\n", face->smoothingGroup);
	printf("Material ID: %d\n", 	face->materialID);

	printf("Face Normal: X = %f, Y = %f, Z = %f\n", face->normal[_X], face->normal[_Y], face->normal[_Z]);
}

/*
 * Function: loadASE_printTFace
 * Description:
 */
static void loadASE_printTFace(ase_mesh_tface_t *tface)
{
	printf("Texture Face ID: %d\n", tface->tfaceID);
	printf("a: %d, b: %d, c: %d\n", tface->a, tface->b, tface->c);
}

/*
 * Function: loadASE_printVertex
 * Description:
 */
static void loadASE_printVertex(ase_mesh_vertex_t *vertex)
{
	printf("Vertex ID: %d\n", vertex->vertexID);

	printf("Coordinates: X = %f, Y = %f, Z = %f\n", vertex->coords[_X],
			vertex->coords[_Y], vertex->coords[_Z]);

	printf("Normal: X = %f, Y = %f, Z = %f\n", vertex->normal[_X],
			vertex->normal[_Y], vertex->normal[_Z]);
}

/*
 * Function: loadASE_printTVertex
 * Description:
 */
static void loadASE_printTVertex(ase_mesh_tvertex_t *tvert)
{
	printf("Texture Vertex ID: %d\n", tvert->vertexID);

	printf("Coordinates: X = %f, Y = %f, Z = %f\n", tvert->coords[_X],
			tvert->coords[_Y], tvert->coords[_Z]);
}

/*
 * Function: loadASE_printMesh
 * Description:
 */
static void loadASE_printMesh(ase_mesh_t *mesh)
{
	int i;

	printf("Vertex Count: %d\n", mesh->numVertex);
	printf("=======================================================\n");

	for(i = 0; i < mesh->numVertex; i++)
		loadASE_printVertex(&(mesh->vertexList[i]));

	printf("Face Count: %d\n", mesh->numFaces);
	printf("=======================================================\n");

	for(i = 0; i < mesh->numFaces; i++)
		loadASE_printFace(&(mesh->faceList[i]));

	printf("Texture Vertex Count: %d\n", mesh->numTVertex);
	printf("=======================================================\n");

	for(i = 0; i < mesh->numTVertex; i++)
		loadASE_printTVertex(&(mesh->tvertList[i]));


	printf("Texture Face Count: %d\n", mesh->numTVFaces);
	printf("=======================================================\n");

	for(i = 0; i < mesh->numTVFaces; i++)
		loadASE_printTFace(&(mesh->tfaceList[i]));
}

/*
 * Function: loadASE_printGeomObject
 * Description:
 */
static void loadASE_printGeomObject(ase_geomObject_t *geomObject)
{
	printf("Name: %s\n", geomObject->name);
	printf("Material Reference: %d\n", geomObject->materialRef);
	loadASE_printMesh(&(geomObject->mesh));
}

/*
 * Function: loadASE_printModel
 * Description:
 */
static void loadASE_printModel(ase_model_t *model)
{
	int i;

	loadASE_printMaterialList(&(model->materials));

	printf("=======================================================\n");

	printf("Object Count: %d\n", model->numObjects);

	for(i = 0; i < model->numObjects; i++)
	{
		printf("=======================================================\n");
		loadASE_printGeomObject(&(model->objects[i]));
	}
}
