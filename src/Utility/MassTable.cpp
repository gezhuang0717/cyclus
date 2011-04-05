// MassTable class
#include <iostream>
#include "MassTable.h"

#include "hdf5.h"
#include "H5Cpp.h" 
#include "H5CompType.h"
#include "H5Exception.h"
#include "GenException.h"
#define MASS_FILE "Data/mass.h5"

using namespace std;
using namespace H5;

MassTable* MassTable::_instance = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MassTable* MassTable::Instance() 
{
	// If we haven't created a MassTable yet, create it, and then and return it
	// either way.
	if (0 == _instance) {
		_instance = new MassTable();
	}
	return _instance;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MassTable::MassTable()
{
  // figure out what's in the file
	initialize();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MassTable::~MassTable()
{
	//Should close the 'mass.h5' file
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Mass MassTable::getMass(Iso tope){
  Mass toRet = nuclide_vec[isoIndex[tope]].mass;
  return toRet;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string MassTable::getName(Iso tope){
};
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MassTable::initialize()
{
  const H5std_string filename = MASS_FILE; 
  const H5std_string groupname = "ame03";
  const H5std_string datasetname = "nuclide";
  const H5std_string A_memb = "A";
  const H5std_string Z_memb = "Z";
  const H5std_string mass_memb = "mass";
  const H5std_string name_memb = "name";
  
  //check if the file is an hdf5 file first.
  if (H5File::isHdf5(MASS_FILE)) {
  }
  else{
    throw GenException("The MASS_FILE is not an hdf5 file.");
  }
  try{
    /*
     * Turn off the auto-printing when failure occurs so that we can
     * handle the errors appropriately
     */
    Exception::dontPrint();
    
    /*
     * Open the file and the dataset.
     */
    H5File* file;
    file = new H5File( filename, H5F_ACC_RDONLY );
    Group* group;
    group = new Group (file->openGroup( groupname ));
    DataSet* dataset;
    dataset = new DataSet (group->openDataSet( datasetname ));
    DataSpace* dataspace;
    dataspace = new DataSpace (dataset->getSpace( ));

    hsize_t dims_out[2];
    int ndims = dataspace->getSimpleExtentDims(dims_out, NULL);
    nuclide_len = dims_out[0];


    /*
     * Create a datatype for nuclide
     */
    CompType mtype( sizeof(nuclide_t) );
    mtype.insertMember( A_memb, HOFFSET(nuclide_t, A), PredType::NATIVE_INT); 
    mtype.insertMember( Z_memb, HOFFSET(nuclide_t, Z), PredType::NATIVE_INT);
    mtype.insertMember( mass_memb, HOFFSET(nuclide_t, mass), PredType::IEEE_F64LE);

    /*
     * Read two fields c and a from s1 dataset. Fields in the file
     * are found by their names "c_name" and "a_name".
     */
    nuclide_t nuclide[nuclide_len];
    dataset->read( nuclide, mtype );

    nuclide_vec.resize(nuclide_len);
    copy(nuclide, nuclide + nuclide_len, nuclide_vec.begin() );
 
    // create a map whose indices are the Iso identifier. 
    Iso Anum;
    Iso Znum;
    Iso tope;
    for(int i = 0; i < nuclide_len; i++){
      Znum = nuclide[i].Z*1000;
      Anum = nuclide[i].A;
      tope = Anum+Znum;
      isoIndex.insert(make_pair(tope, i));
    };

    /*
     * Display the fields
     */
//    cout << endl << "Field A : " << endl;
//    for(int i = 0; i < nuclide_len; i++)
//      cout << nuclide_vec[i].A << " ";
//    cout << endl;
//
//    cout << endl << "Field Z : " << endl;
//    for(int i = 0; i < nuclide_len; i++)
//      cout << nuclide_vec[i].Z << " ";
//    cout << endl;
//
//    cout << endl << "Field mass : " << endl;
//    for(int i = 0; i < nuclide_len; i++)
//      cout << nuclide_vec[i].mass << " ";
//    cout << endl;

    delete dataspace;
    delete group;
    delete dataset;
    delete file;
  }

  // catch failure caused by the H5File operations
  catch( FileIException error )
  {
     error.printError();
  }
 
  // catch failure caused by the Group operations
  catch( GroupIException error )
  {
     error.printError();
  }
 
  // catch failure caused by the DataSet operations
  catch( DataSetIException error )
  {
     error.printError();
  }
 
  // catch failure caused by the DataSpace operations
  catch( DataSpaceIException error )
  {
     error.printError();
  }
 
  // catch failure caused by the DataType operations
  catch( DataTypeIException error )
  {
     error.printError();
  }
};
