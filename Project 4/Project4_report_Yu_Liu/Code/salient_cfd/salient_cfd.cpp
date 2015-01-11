#include <vtkSmartPointer.h>
#include <vtkDataSetReader.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkStructuredPoints.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty2D.h>
#include <vtkActor.h>
#include <vtkContourFilter.h>
#include <vtkVolumeProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCommand.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSliderWidget.h>
#include <vtkProperty.h>

int main (int argc, char* argv[])
{
	vtkSmartPointer<vtkDataSetReader> Reader = vtkSmartPointer<vtkDataSetReader>::New();
	if(argc ==2)
		Reader->SetFileName(argv[1]);	
	else 
		exit(0);

	vtkSmartPointer<vtkColorTransferFunction> Color =  vtkSmartPointer<vtkColorTransferFunction>::New();
	Color->AddRGBPoint(2500, 1.0, 0.0, 0.0, 0.0, 1.0);
	Color->AddRGBPoint(11000, 0.0, 1.0, 0.0, 0.0, 1.0);
	Color->AddRGBPoint(25000, 0.0, 0.0, 1.0, 0.0, 1.0);
	Color->AddRGBPoint(50000, 1.0, 1.0, 1.0, 0.0, 1.0);

	vtkSmartPointer<vtkContourFilter> cf = vtkSmartPointer<vtkContourFilter>::New();
	cf->SetInput(Reader->GetStructuredPointsOutput()); 
	cf->SetValue(0, 2500);
	cf->Update();

	vtkSmartPointer<vtkContourFilter> cf1 = vtkSmartPointer<vtkContourFilter>::New();
	cf1->SetInput(Reader->GetStructuredPointsOutput()); 
	cf1->SetValue(0, 11000);
	cf1->Update();

	vtkSmartPointer<vtkContourFilter> cf2 = vtkSmartPointer<vtkContourFilter>::New();
	cf2->SetInput(Reader->GetStructuredPointsOutput()); 
	cf2->SetValue(0, 25000);
	cf2->Update();

	vtkSmartPointer<vtkContourFilter> cf3 = vtkSmartPointer<vtkContourFilter>::New();
	cf3->SetInput(Reader->GetStructuredPointsOutput()); 
	cf3->SetValue(0, 50000);
	cf3->Update();

	vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetLookupTable(Color);
	mapper->SetInput(cf->GetOutput());
	mapper->ImmediateModeRenderingOn();

	vtkSmartPointer<vtkDataSetMapper> mapper1 = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper1->SetLookupTable(Color);
	mapper1->SetInput(cf1->GetOutput());
	mapper1->ImmediateModeRenderingOn();

	vtkSmartPointer<vtkDataSetMapper> mapper2 = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper2->SetLookupTable(Color);
	mapper2->SetInput(cf2->GetOutput());
	mapper2->ImmediateModeRenderingOn();

	vtkSmartPointer<vtkDataSetMapper> mapper3 = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper3->SetLookupTable(Color);
	mapper3->SetInput(cf3->GetOutput());
	mapper3->ImmediateModeRenderingOn();

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->GetProperty()->SetOpacity(0.1);
	actor->SetMapper(mapper);

	vtkSmartPointer<vtkActor> actor1 = vtkSmartPointer<vtkActor>::New();
	actor1->GetProperty()->SetOpacity(0.3);
	actor1->SetMapper(mapper1);

	vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
	actor2->GetProperty()->SetOpacity(0.7);
	actor2->SetMapper(mapper2);

	vtkSmartPointer<vtkActor> actor3 = vtkSmartPointer<vtkActor>::New();
	actor3->GetProperty()->SetOpacity(1.0);
	actor3->SetMapper(mapper3);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetAlphaBitPlanes(1);
	renderWindow->SetMultiSamples(0);

	renderer->AddActor(actor);
	renderer->AddActor(actor1);
	renderer->AddActor(actor2);
	renderer->AddActor(actor3);

	renderer->UseDepthPeelingOn();
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->ResetCamera();
	renderer->ResetCameraClippingRange();

	vtkCamera* cam1 = renderer->GetActiveCamera();

	vtkSmartPointer<vtkSliderWidget> sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
	sliderWidget->SetInteractor(renderWindowInteractor);
	sliderWidget->SetAnimationModeToAnimate();
	sliderWidget->EnabledOn();

	renderWindowInteractor->Initialize();
	renderWindow->Render();
	int depthPeelingWasUsed=renderer->GetLastRenderingUsedDepthPeeling();
	cout<<depthPeelingWasUsed<<endl;

	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}