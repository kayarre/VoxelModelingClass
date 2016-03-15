#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkSphere.h>
#include <vtkSampleFunction.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolumeProperty.h>
#include <vtkCamera.h>
#include <vtkImageShiftScale.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkXMLImageDataReader.h>

#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkCellDataToPointData.h>
#include <vtkImageShiftScale.h>
#include <vtkExtractVOI.h>
#include <vtkImageCast.h>
#include <vtkDoubleArray.h>
#include <iostream>


 
static void CreateImageData(vtkImageData* im);
 
int main(int argc, char *argv[])
{
  vtkSmartPointer<vtkImageData> imageData =
    vtkSmartPointer<vtkImageData>::New();
    
  if (argc < 2)
    {
    CreateImageData(imageData);
    }
  else
    {
    vtkSmartPointer<vtkXMLImageDataReader> reader = 
      vtkSmartPointer<vtkXMLImageDataReader>::New();
    reader->SetFileName(argv[1]);
    reader->Update();  
    imageData = reader->GetOutput();
    }
  
  std::cout << imageData->GetCellType() << std::endl;
  std::cout << imageData->GetScalarTypeAsString() << std::endl;
  // Now check for cell data
  vtkCellData *cd = imageData->GetCellData();
  
  
  int* inputDims = imageData->GetDimensions();
  std::cout << "Dims: " << " x: " << inputDims[0]
                        << " y: " << inputDims[1]
                        << " z: " << inputDims[2] << std::endl;
  std::cout << "Number of points: " << imageData->GetNumberOfPoints() << std::endl;
  std::cout << "Number of cells: " << imageData->GetNumberOfCells() << std::endl;
  std::cout << "scalar components: " << imageData->GetNumberOfScalarComponents() << std::endl; 
 
  vtkSmartPointer<vtkExtractVOI> extractVOI =
      vtkSmartPointer<vtkExtractVOI>::New();
  extractVOI->SetInputData(imageData);
  extractVOI->SetVOI(inputDims[0]/8.,3.*inputDims[0]/8.,inputDims[1]/8.,3.*inputDims[1]/8., inputDims[2]/8.,3.*inputDims[2]/8.);
  extractVOI->Update();
  
  vtkImageData* extracted = extractVOI->GetOutput();
  
  int* extractedDims = extracted->GetDimensions();
  std::cout << "Dims: " << " x: " << extractedDims[0]
                        << " y: " << extractedDims[1]
                        << " z: " << extractedDims[2] << std::endl;
  std::cout << "Number of points: " << extracted->GetNumberOfPoints() << std::endl;
  std::cout << "Number of cells: " << extracted->GetNumberOfCells() << std::endl;
  
  
  double min = extracted->GetScalarRange()[0];
  double max = extracted->GetScalarRange()[1];
  std::cout << "valuesRange = " << min << " " << max << std::endl;
 
  /*
  vtkSmartPointer<vtkImageShiftScale> shift =
    vtkSmartPointer<vtkImageShiftScale>::New();
    
  shift->SetShift(0);
  shift->SetScale(255.0);
  shift->SetOutputScalarTypeToUnsignedChar();
  std::cout << "Here  range" << std::endl;
  
  shift->SetInputData( extracted);
  
  
  std::cout << "cast" << std::endl;
  */
 
  vtkSmartPointer<vtkRenderWindow> renWin = 
    vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkRenderer> ren1 = 
    vtkSmartPointer<vtkRenderer>::New();
  ren1->SetBackground(0.1,0.4,0.2);
 
  renWin->AddRenderer(ren1);
 
  renWin->SetSize(601,800); // intentional odd and NPOT  width/height
 
  vtkSmartPointer<vtkRenderWindowInteractor> iren = 
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);
 
  renWin->Render(); // make sure we have an OpenGL context.

 
  vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = 
    vtkSmartPointer<vtkSmartVolumeMapper>::New();
  volumeMapper->SetBlendModeToComposite(); // composite first
//#if VTK_MAJOR_VERSION <= 5
//  volumeMapper->SetInputConnection(imageData->GetProducerPort());
//#else
  volumeMapper->SetInputData(extracted);
//#endif  
  vtkSmartPointer<vtkVolumeProperty> volumeProperty = 
    vtkSmartPointer<vtkVolumeProperty>::New();
  volumeProperty->ShadeOff();
  volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);
 
  vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity = 
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  compositeOpacity->AddPoint(0.0,0.0);
  compositeOpacity->AddPoint(80.0,1.0);
  compositeOpacity->AddPoint(80.1,0.0);
  compositeOpacity->AddPoint(255.0,0.0);
  volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.
 
  vtkSmartPointer<vtkColorTransferFunction> color = 
    vtkSmartPointer<vtkColorTransferFunction>::New();
  color->AddRGBPoint(   0, 1.0, 1.0, 1.0 );
  color->AddRGBPoint( 255, 1.0, 1.0, 1.0 );
  //color->AddRGBPoint(0.0  ,0.0,0.0,1.0);
  //color->AddRGBPoint(40.0  ,1.0,0.0,0.0);
  //color->AddRGBPoint(255.0,1.0,1.0,1.0);
  volumeProperty->SetColor(color);
 
  vtkSmartPointer<vtkVolume> volume = 
    vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  ren1->AddViewProp(volume);
  ren1->ResetCamera();
  std::cout <<" got here" << std::endl;
  // Render composite. In default mode. For coverage.
  renWin->Render();
 
  // 3D texture mode. For coverage.
//#if !defined(VTK_LEGACY_REMOVE)
//  volumeMapper->SetRequestedRenderModeToRayCastAndTexture();
//#endif // VTK_LEGACY_REMOVE
  std::cout <<" got here" << std::endl;
  renWin->Render();
 
  // Software mode, for coverage. It also makes sure we will get the same
  // regression image on all platforms.
  volumeMapper->SetRequestedRenderModeToRayCast();
  renWin->Render();
 
  iren->Start();
 
  return EXIT_SUCCESS;
}
 
void CreateImageData(vtkImageData* imageData)
{
  // Create a spherical implicit function.
  vtkSmartPointer<vtkSphere> sphere =
    vtkSmartPointer<vtkSphere>::New();
  sphere->SetRadius(0.1);
  sphere->SetCenter(0.0,0.0,0.0);
 
  vtkSmartPointer<vtkSampleFunction> sampleFunction =
    vtkSmartPointer<vtkSampleFunction>::New();
  sampleFunction->SetImplicitFunction(sphere);
  sampleFunction->SetOutputScalarTypeToDouble();
  sampleFunction->SetSampleDimensions(127,127,127); // intentional NPOT dimensions.
  sampleFunction->SetModelBounds(-1.0,1.0,-1.0,1.0,-1.0,1.0);
  sampleFunction->SetCapping(false);
  sampleFunction->SetComputeNormals(false);
  sampleFunction->SetScalarArrayName("values");
  sampleFunction->Update();
 
  vtkDataArray* a = sampleFunction->GetOutput()->GetPointData()->GetScalars("values");
  double range[2];
  a->GetRange(range);
 
  vtkSmartPointer<vtkImageShiftScale> t = 
    vtkSmartPointer<vtkImageShiftScale>::New();
  t->SetInputConnection(sampleFunction->GetOutputPort());
 
  t->SetShift(-range[0]);
  double magnitude=range[1]-range[0];
  if(magnitude==0.0)
    {
    magnitude=1.0;
    }
  t->SetScale(255.0/magnitude);
  t->SetOutputScalarTypeToUnsignedChar();
 
  t->Update();
 
  imageData->ShallowCopy(t->GetOutput());
}
