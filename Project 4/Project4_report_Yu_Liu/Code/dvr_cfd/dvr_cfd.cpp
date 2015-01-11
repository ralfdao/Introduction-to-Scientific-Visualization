#include <vtkSmartPointer.h>
#include <vtkDataSetReader.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkStructuredPoints.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty2D.h>
#include <vtkActor.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCommand.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkProperty.h>

int main (int argc, char* argv[])
{
	vtkSmartPointer<vtkDataSetReader> Reader = vtkSmartPointer<vtkDataSetReader>::New();
	if(argc ==2)
		Reader->SetFileName(argv[1]);	
	else 
		exit(0);

	vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =  vtkSmartPointer<vtkColorTransferFunction>::New();
	colorTransferFunction->AddRGBPoint(2400, 0.0, 0.0, 0.0);
	colorTransferFunction->AddRGBPoint(2500, 1.0, 0.0, 0.0);
	colorTransferFunction->AddRGBPoint(11000, 0.0, 1.0, 0.0);
	colorTransferFunction->AddRGBPoint(25000, 0.0, 0.0, 1.0);
	colorTransferFunction->AddRGBPoint(50000, 1.0, 1.0, 1.0);

	vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
	opacityTransferFunction->AddPoint(2400, 0.0);
	opacityTransferFunction->AddPoint(2500, 0.006);
	opacityTransferFunction->AddPoint(11000, 0.006);
	opacityTransferFunction->AddPoint(25000, 0.02);
	opacityTransferFunction->AddPoint(50000, 0.6);

	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetColor(colorTransferFunction);
	volumeProperty->SetScalarOpacity(opacityTransferFunction);
	volumeProperty->ShadeOn();
	volumeProperty->SetInterpolationTypeToLinear();

	vtkSmartPointer<vtkVolumeRayCastCompositeFunction> compositeFunction = vtkSmartPointer<vtkVolumeRayCastCompositeFunction>::New();
	vtkSmartPointer<vtkVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkVolumeRayCastMapper>::New();
	volumeMapper->SetVolumeRayCastFunction(compositeFunction);
	double distance = volumeMapper->GetSampleDistance();
	cout<<distance<<endl;
	volumeMapper->SetSampleDistance(0.2);
	volumeMapper->SetInputConnection(Reader->GetOutputPort());

	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();

	renderer->AddVolume(volume);
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->ResetCamera();
	renderer->ResetCameraClippingRange();

	vtkCamera* cam1 = renderer->GetActiveCamera();

	renderWindowInteractor->Initialize();
	renderWindow->Render();

	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}