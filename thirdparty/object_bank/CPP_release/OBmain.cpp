#include "OBmain.h"
#include <time.h>
#include"mymex.h"
#include <setjmp.h>

using namespace std;

string INPUTDIR;
string OUTPUTDIR;

const int g_MIN_EDGE_LEN = 400;

//-------------------GET DIRECTORY--------------------------------------------------------------//
//For a given directory, reads all filenames and outputs the values in a vector
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL)
        files.push_back(string(dirp->d_name));
    closedir(dp);

    /* Remove the first two entries in the vector*/
    files.erase(files.begin(),files.begin()+2);

    return 0;
}

//-------------------LOAD MODELS-----------------------------------------------------------------//
//Takes a filename, loads the jpeg using libjpeg, then returns an mxarray filled with the 8-bit values
//current cast as doubles. This will likely take some debugging
int loadModelFiles(string dir, vector<CModel>& models, string mvalue){
    vector<string> modelfiles;

    if(mvalue.size()==0) {
        getdir ("./models/", modelfiles);
	int sz=modelfiles.size();
	for (int i=0; i<sz; i++) modelfiles[i] = "./models/" + modelfiles[i];
    } else {
	ifstream fin(mvalue.c_str());
	string curr;
	while(fin>>curr) modelfiles.push_back(curr);
    }

    string filename;
    size_t found;

    for (int i = 0; i < modelfiles.size(); i++)  {
        filename = modelfiles[i];
        cout << "Model " << i+1 << ": " <<filename<<endl;
        found = filename.find("text");

        if(found>0 && found<filename.length() ){
            CModel model(filename); 
            models.push_back(model);
        }
    }

    return 1;
}

struct my_error_mgr {
    struct jpeg_error_mgr pub;	/* "public" fields */

    jmp_buf setjmp_buffer;		/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

    void
my_error_exit (j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}


//-------------------LOAD IMAGE------------------------------------------------------------------//
//Takes a filename, loads the jpeg using libjpeg, then returns an mxarray filled with the 8-bit values
//current cast as doubles. This will likely take some debugging
mxArray * loadImg(const char *filename)
{

    /* we will be using this uninitialized pointer later to store raw, uncompressd image */
    unsigned char *raw_image = NULL;

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    //struct jpeg_error_mgr jerr;
    struct my_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen( filename, "rb" );
    unsigned long location = 0;
    int i = 0;

    if ( !infile )
    {
        throw "Error opening jpeg file.";
        return NULL;
    }
    int rv = 0;

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error( &jerr.pub );
    jerr.pub.error_exit = my_error_exit;
    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_decompress(&cinfo);
        throw "Error processing jpeg file";
        return NULL;
    }

    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress( &cinfo );
    /* this makes the library read from infile */
    jpeg_stdio_src( &cinfo, infile );
    /* reading the image header which contains image information */
    rv = jpeg_read_header( &cinfo, TRUE );

    /* Start decompression jpeg here */
    jpeg_start_decompress( &cinfo );
    if (cinfo.num_components != 3) {
        cerr << "only accept 3-channel image" << endl;
        exit(-1);
    } 

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char*)malloc( cinfo.output_width*cinfo.output_height*cinfo.num_components );
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );
    /* read one scan line at a time */
    while( cinfo.output_scanline < cinfo.image_height )
    {
        jpeg_read_scanlines( &cinfo, row_pointer, 1 );
        for( i=0; i<cinfo.image_width*cinfo.num_components;i++)
            raw_image[location++] = row_pointer[0][i];
    }

    //NOW CONVERT THE RAW IMAGE INTO A MXARRAY

    int dims[3];
    dims[0] = (int)cinfo.image_height;
    dims[1] = (int)cinfo.image_width;
    dims[2] = 3;

    int widthStep = cinfo.image_width*cinfo.num_components;
    vector<mxArray *> Array;
    mxArray *mxarray = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);
    Array.push_back(mxarray);
    double *parray = (double *)mxGetPr(mxarray), temp = 0;
    for (int c = 0; c < cinfo.num_components; c++)
        for (int x = 0; x < cinfo.image_width; x++)
            for (int y = 0; y < cinfo.image_height; y++)
            {
                if (cinfo.num_components == 1)
                {
                    *(parray++) = raw_image[y * cinfo.image_width*cinfo.num_components + cinfo.num_components * x] / 255.0;
                    temp = raw_image[y * cinfo.image_width*cinfo.num_components + cinfo.num_components * x] / 255.0;
                }
                else
                {
                    *(parray++) = raw_image[y * cinfo.image_width*cinfo.num_components + cinfo.num_components * x +c] / 255.0;
                    temp = raw_image[y * cinfo.image_width*cinfo.num_components + cinfo.num_components * x +c] / 255.0;
                }
            } 

    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress( &cinfo );
    jpeg_destroy_decompress( &cinfo );
    free( row_pointer[0] );
    free(raw_image);
    fclose( infile );
    return mxarray; 
}

int WriteToDisk(vector<mxArray *> & vecMatrix, string szFileName)
{ 
    int vecSize = vecMatrix.size();
    FILE * fp = fopen(szFileName.c_str(), "w");
    if (fp == NULL) cout << "Cannot create output file!";
    else { 
        fprintf(fp, "%d\n", vecSize);
        for (int i = 0; i < vecSize; i++) {
            if (vecMatrix[i]->NDim != 2)
                cout << "matrix dimension incorrect" << endl;
            int nrow = vecMatrix[i]->Dims[0], ncol = vecMatrix[i]->Dims[1];
            fprintf(fp, "%d %d\n", nrow, ncol); 
            double val;
            for (int x = 0; x < ncol; x++) {
                for (int y = 0; y < nrow; y++) fprintf(fp, "%lf ", vecMatrix[i]->get2D(y, x, val));
                fprintf(fp, "\n");
            }
        }
        fclose(fp);
    }
    return 1;
}

int WriteFeatVec(vector<float> & featvec, string szFileName)
{ 
    int vecSize = featvec.size();
    cout << "output: " << szFileName << endl;
    FILE * fp = fopen(szFileName.c_str(), "w");
    if (fp == NULL) cout << "Cannot create output file!";
    else { 
        for (int i = 0; i < vecSize; i++) fprintf(fp, "%lf \n", featvec[i]);
        fclose(fp);
    }
    return 1;
}

int ClearMXVec(vector<mxArray*> & vec)
{ 
    int vecSize = vec.size();
    for (int i = 0; i < vecSize; i++) delete vec[i];	
    vec.clear();
    return 1;
}

int extractOBFeature(mxArray * input, vector<CModel> &modelList, bool saveMap, string fileName, vector<float> & outputFeature, vector<mxArray *> & responsemap, int numComponents, int numLevels)
{
    int nLevel = 3; //Number of SPM levels
    int spmFeatureSize=0, curr = 1;
    for (int i=0; i<nLevel; i++) {
	spmFeatureSize+=curr*curr;
	curr = curr<<1;
    }

    int nrow; int ncol;
    float featurePerMap[spmFeatureSize];

    int nmaps;

    //Allocate memory for loops
    double temp;

    //All the models in the 177 detectors have sbin=8, interval=10, so we calculate the feature
    //pyramid per image, rather than per detection. This requires hardcoding parameters:
    int interval = 10;
    int sbin = 8;

    vector<mxArray *> feat; 
    vector<double> scales;

    float rmap[input->Dims[0]* input->Dims[1]];    

    ClearMXVec(feat); 
    scales.clear();
    outputFeature.clear();
    char savesuffix[30];

    featpyramid(input, sbin, interval, feat, scales); // build feature pyramid

    //RUN EACH DETECTOR---------------------------------------------------------
    cout << "Running Detection: " << endl;

    int nmaps_max = numLevels*numComponents, mod_skip = 10;
    for (int detId = 0; detId < modelList.size(); detId++) {
        ClearMXVec(responsemap);
        detect_postfeatpyr(feat, scales, modelList[detId], responsemap, numComponents);

        //Extract 2D maps from the vecResponseMap and pass it inot the MaxGetSpaticalPyramid function
        nmaps =  responsemap.size();

        if (nmaps > nmaps_max) nmaps = nmaps_max;
        for(int i=0; i<nmaps; ++i) {
            nrow = responsemap[i]->Dims[0], ncol = responsemap[i]->Dims[1];
            for (int y = 0; y < nrow; y++) {			
                for (int x = 0; x < ncol; x++) {
                    responsemap[i]->get2D(y,x,temp); 
                    rmap[x+y*ncol] = float(temp);
                }
            }

            MaxGetSpatialPyramid(rmap,nLevel,featurePerMap,nrow,ncol);

            //Insert featurePerMap element-by-element into the output vector
            for(int f=0; f<spmFeatureSize; f++) outputFeature.push_back(featurePerMap[f]);
        }//end loop over nmaps 

        if (detId % mod_skip == 1) cout << "." << flush;
        if (saveMap) {
            sprintf(savesuffix,".%d.map",detId);
            WriteToDisk(responsemap, OUTPUTDIR + fileName + savesuffix);
        }

    } //End loop over detectors 
}



//-------------------   MAIN   ------------------------------------------------------------------//
//The Entry point of the application
int main(int argc, char *argv[]){

    //INPUT HANDLING-----------------------------------------------------------------------------
    if ( argc < 3) // argc should be 2 for correct execution
    {  // We print argv[0] assuming it is the program name
        cout<<"\nERROR: Proper usage is "<< argv[0] << " [optional args, see README] <input directory>/ <outputdir>/ \n";
        cout<<"All jpeg files in input directory will be processed and placed in the output. Note that the trailing slashes are necessary.\n\n";

        return 0; 
    }

    int rflag = 0;
    int mflag = 0;
    int fflag = 1; 	//The system writes out feature vectors by default

    int c;
    char *rvalue = NULL;
    string mvalue;
    char *fvalue = NULL;
    int numLevels = 6;			//DEFAULT: 6 scale levels
    int numComponents = 2;		//DEFAULT: 2 Components
    int index;

    opterr = 0;

    while ((c = getopt (argc, argv, "R:M:F:L:")) != -1)
        switch (c)
        {
            case 'R':
                rflag = (int) atol(optarg);
                break;
            case 'F':
                fflag = (int) atol(optarg);
                break;
            case 'M':
                mflag = 1;
                mvalue = optarg;		//Reads in a list of modelfiles
                break;
            case 'L':
                numLevels = (int) atol(optarg);
                break;    
            default:
                cout << "Input parameters not recognized " << endl;
                break;
        }

    INPUTDIR = (string) argv[optind];
    OUTPUTDIR = (string) argv[optind+1];

    if (!(rflag | fflag)){
        cout << "Specify output type(s): -R creates responsemaps and -F creates SPM-based feature vectors";
        return 0;
    }

    vector <CModel> modelList;  	//list of all models in the models/ dir are stored here
    vector <string> fileNames; 		//list of all images to process are stored here

    vector<mxArray *> responsemap;	//outputs of detect() are stored here
    mxArray * input_resized;

    string imgName;
    string currDir(INPUTDIR);
    mxArray *im = NULL;
    int min_edge_len;
    double ratio;

    //output
    vector<float> outputFeature;

    size_t found;

    //LOAD THE IMAGES-----------------------------------------------------------------------------
    getdir(currDir, fileNames);       

    //LOAD THE MODELS-----------------------------------------------------------------------------
    loadModelFiles(currDir, modelList, mvalue);
    cout << " Done!" << endl;

    //LOAD EACH IMAGE-----------------------------------------------------------------------------
    for (int i = 0; i < fileNames.size(); i++) 
    {
        imgName = INPUTDIR + fileNames[i]; 
        cout << "Loading image "<< imgName << " ... " <<  flush;
        //continue;
        try {
            im = loadImg(imgName.c_str());
            cout << " Done." <<endl <<flush;

            min_edge_len = min(im->Dims[0], im->Dims[1]);

            ratio = (float)g_MIN_EDGE_LEN / min_edge_len;		
            input_resized = resize(im, ratio);

            //Check if output file for this file already exists:

            FILE * ifile = fopen((OUTPUTDIR + fileNames[i] + ".feat").c_str(), "r");;
            if( ifile==0)
            {
                //******************THE CORE FUNCTION CALL********************************//
                extractOBFeature(input_resized, modelList, rflag, fileNames[i], outputFeature, responsemap,numComponents, numLevels);
                                //************************************************************************//
                if (fflag)
                    WriteFeatVec(outputFeature, OUTPUTDIR + fileNames[i] + ".feat");
            }
            else
            {
                cout << "Skipping redundant file." << endl;
                fclose(ifile);
            }
            mxFree(&im);
            mxFree(&input_resized);
        }

        catch (const char * new_error){
            cout << "ERROR: '" << imgName << "' did not load. " << endl ;
            cout << new_error << endl;
        }

    }	//end loops around input directories

    return 0;
} //end main

