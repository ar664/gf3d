#include "gf3d_object.h"

#include "simple_logger.h"

#define OBJECT_MAX 1024

static Object ObjectList[OBJECT_MAX];

static void object_close();

void gf3d_object_init()
{
    memset(ObjectList,0,sizeof(Object)*OBJECT_MAX);
    atexit(object_close);
}


Object *object_new()
{
    int i;
    for (i = 0; i < OBJECT_MAX; i++)
    {
        if (ObjectList[i].ref_count == 0)
        {
            memset(&ObjectList[i],0,sizeof(Object));
            ObjectList[i].ref_count = 1;
            return &ObjectList[i];
        }
    }
    return NULL;
}

Object *object_get_by_filename(char *filename)
{
    int i;
    for (i = 0; i < OBJECT_MAX; i++)
    {
        if ((ObjectList[i].ref_count != 0) &&
            (strcmp(ObjectList[i].filename,filename) == 0))
        {
            return &ObjectList[i];
        }
    }
    return NULL;
}

void object_file_get_counts(Object* model, FILE* file)
{
    char buf[1024];
    int  numvertices = 0;
    int  numnormals = 0;
    int  numtexcoords = 0;
    int  numfaces = 0;
    
    if ((file == NULL) ||
        (model == NULL))
    {
        return;
    }
    while(fscanf(file, "%s", buf) != EOF)
    {
        switch(buf[0])
        {
            case 'v':
                switch(buf[1])
                {
                    case '\0':
                        fgets(buf, sizeof(buf), file);
                        numvertices++;
                        break;
                    case 'n':
                        fgets(buf, sizeof(buf), file);
                        numnormals++;
                        break;
                    case 't':
                        fgets(buf, sizeof(buf), file);
                        numtexcoords++;
                        break;
                    default:
                        break;
                }
                break;
            case 'f':
                fgets(buf, sizeof(buf), file);
                numfaces++;
                break;
            default:
                fgets(buf, sizeof(buf), file);
                break;
        }
    }
    model->num_triangles = numfaces;
    model->num_vertices = numvertices;
    model->num_texels = numtexcoords;
    model->num_normals = numnormals;
}

void object_allocate(Object *model)
{
    if (!model)return;
    if (model->num_vertices)
    {
        model->vertex_array = malloc(sizeof(double)*3*model->num_vertices);
        if (model->vertex_array)
        {
            memset(model->vertex_array,0,sizeof(double)*3*model->num_vertices);
        }
    }
    if (model->num_normals)
    {
        model->normal_array = malloc(sizeof(double)*3*model->num_normals);
        if (model->normal_array)
        {
            memset(model->normal_array,0,sizeof(double)*3*model->num_normals);
        }
    }
    if (model->num_texels)
    {
        model->texel_array = malloc(sizeof(double)*2*model->num_texels);
        if (model->texel_array)
        {
            memset(model->texel_array,0,sizeof(double)*2*model->num_texels);
        }
    }
    if (model->num_triangles)
    {
        model->triangle_array = malloc(sizeof(Triangle)*model->num_triangles);
        if (model->triangle_array)
        {
            memset(model->triangle_array,0,sizeof(Triangle)*model->num_triangles);
        }
    }
}

void object_file_parse(Object * model, FILE* file)
{
    int  numvertices = 0;
    int  numnormals = 0;
    int  numtexcoords = 0;
    int  numfaces = 0;
    char buf[128];
    double x,y,z;
    uint32_t t1,t2,t3,v1,v2,v3,n1,n2,n3;
        
    if (model == NULL)
    {
        return;
    }
    if (file == NULL)
    {
        return;
    }
    rewind(file);
    while(fscanf(file, "%s", buf) != EOF)
    {
        switch(buf[0])
        {
            case 'v':
                switch(buf[1])
                {
                    case '\0':
                        fscanf(
                            file,
                            "%lf %lf %lf",
                            &x,
                            &y,
                            &z
                        );
                        model->vertex_array[numvertices++] = x;
                        model->vertex_array[numvertices++] = y;
                        model->vertex_array[numvertices++] = z;
                        break;
                    case 'n':
                        fscanf(
                            file,
                            "%lf %lf %lf",
                            &x,
                            &y,
                            &z
                        );
                        model->normal_array[numnormals++] = x;
                        model->normal_array[numnormals++] = y;
                        model->normal_array[numnormals++] = z;
                        break;
                    case 't':
                        fscanf(
                            file,
                            "%lf %lf",&x,&y
                        );
                        model->texel_array[numtexcoords++] = x;
                        model->texel_array[numtexcoords++] = 1 - y;
                        break;
                    default:
                        break;
                }
                break;
            case 'f':
                fscanf(
                    file,
                    "%u/%u/%u %u/%u/%u %u/%u/%u",
                    &v1,
                    &t1,
                    &n1,
                    &v2,
                    &t2,
                    &n2,
                    &v3,
                    &t3,
                    &n3);
                model->triangle_array[numfaces].p[0] = (Truple){(v1 - 1),
                                                                (t1 -1), 
                                                                (n1 -1)};
                model->triangle_array[numfaces].p[1] = (Truple){(v2 - 1),
                                                                (t2 -1), 
                                                                (n2 -1)};
                model->triangle_array[numfaces].p[2] = (Truple){(v3 - 1),
                                                                (t3 -1), 
                                                                (n3 -1)};
                numfaces++;
                break;
            default:
                /*chomp the line*/
                fgets(buf, sizeof(buf), file);
                break;
        }
    }
}

Object *gf3d_object_load(char *filename)
{
    FILE *file;
    Object *objFile;
    
    objFile = object_get_by_filename(filename);
    if (objFile)
    {
        objFile->ref_count++;
        return objFile;
    }
    
    objFile = object_new();
    if (!objFile)
    {
        return NULL;
    }
    
    file = fopen(filename,"r");
    if (file == NULL)
    {
        slog("failed to open file %s",filename);
        return NULL;
    }
    
    
    object_file_get_counts(objFile,file);
    
    slog("vertices: %i",objFile->num_vertices);
    slog("normals: %i",objFile->num_normals);
    slog("texels: %i",objFile->num_texels);
    slog("faces: %i",objFile->num_triangles);
    
    object_allocate(objFile);
    object_file_parse(objFile, file);
    
    fclose(file);
    
    return objFile;
}

void gf3d_object_delete(Object *obj)
{
    if (!obj)return;
    
    if (obj->vertex_array)
    {
        free(obj->vertex_array);
    }
    if (obj->texel_array)
    {
        free(obj->texel_array);
    }
    if (obj->normal_array)
    {
        free(obj->normal_array);
    }
    if (obj->triangle_array)
    {
        free(obj->triangle_array);
    }
    memset(obj,0,sizeof(Object));
}

static void object_close()
{
    int i;
    for (i = 0; i < OBJECT_MAX; i++)
    {
        if (ObjectList[i].ref_count)
        {
            gf3d_object_delete(&ObjectList[i]);
        }
    }
}

