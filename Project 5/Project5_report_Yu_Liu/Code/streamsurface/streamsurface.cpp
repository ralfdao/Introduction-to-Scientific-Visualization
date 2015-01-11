#include <vtkSmartPointer.h>
#include <vtkDataSetReader.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkStructuredPoints.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkWidgetEvent.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkRungeKutta4.h>
#include <vtkLineSource.h>
#include <vtkStreamLine.h>
#include <vtkRuledSurfaceFilter.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>

int main (int argc, char* argv[])
{
	vtkSmartPointer<vtkDataSetReader> Delta_Wing_Geometry_Reader = vtkSmartPointer<vtkDataSetReader>::New();
	vtkSmartPointer<vtkDataSetReader> VectorReader = vtkSmartPointer<vtkDataSetReader>::New();
	if(argc ==3)
	{
		Delta_Wing_Geometry_Reader->SetFileName(argv[1]);
		VectorReader->SetFileName(argv[2]);
	}
	else 
		exit(0);

	vtkSmartPointer<vtkDataSetMapper>Delta_Wing_Geometry_Mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	Delta_Wing_Geometry_Mapper->SetInputConnection(Delta_Wing_Geometry_Reader->GetOutputPort());

	vtkSmartPointer<vtkActor> Delta_Wing_Geometry_Actor = vtkSmartPointer<vtkActor>::New();
	Delta_Wing_Geometry_Actor->SetMapper(Delta_Wing_Geometry_Mapper);
	Delta_Wing_Geometry_Actor->GetProperty()->SetColor(0.5, 0.5, 1.0);

	VectorReader->Update();

	vtkSmartPointer<vtkLineSource> rake = vtkSmartPointer<vtkLineSource>::New();
	rake->SetPoint1(45,-45,5);
	rake->SetPoint2(45,45,5);
	rake->SetResolution(16);

	vtkSmartPointer<vtkRungeKutta4> integ = vtkSmartPointer<vtkRungeKutta4>::New();
	vtkSmartPointer<vtkStreamLine> streamer1 = vtkSmartPointer<vtkStreamLine>::New();
	streamer1->SetInputConnection(VectorReader->GetOutputPort());
	streamer1->SetSource(rake->GetOutput());
	streamer1->SetIntegrator(integ);
	streamer1->SetMaximumPropagationTime(4);
	streamer1->SetStepLength(0.01);
	streamer1->SetIntegrationStepLength(0.01);
	streamer1->SetIntegrationDirectionToBackward();
	streamer1->VorticityOn();
	streamer1->SpeedScalarsOn();

	vtkSmartPointer<vtkStreamLine> streamer2 = vtkSmartPointer<vtkStreamLine>::New();
	streamer2->SetInputConnection(VectorReader->GetOutputPort());
	streamer2->SetSource(rake->GetOutput());
	streamer2->SetIntegrator(integ);
	streamer2->SetMaximumPropagationTime(4);
	streamer2->SetStepLength(0.01);
	streamer2->SetIntegrationStepLength(0.01);
	streamer2->SetIntegrationDirectionToForward();
	streamer2->VorticityOn();
	streamer2->SpeedScalarsOn();

	vtkSmartPointer<vtkRuledSurfaceFilter> scalarSurface1 = vtkSmartPointer<vtkRuledSurfaceFilter>::New();
	scalarSurface1->SetInputConnection(streamer1->GetOutputPort());
	scalarSurface1->SetResolution(50, 50);
	scalarSurface1->SetRuledModeToResample();

	vtkSmartPointer<vtkRuledSurfaceFilter> scalarSurface2 = vtkSmartPointer<vtkRuledSurfaceFilter>::New();
	scalarSurface2->SetInputConnection(streamer2->GetOutputPort());
	scalarSurface2->SetResolution(50, 50);
	scalarSurface2->SetRuledModeToResample();

	vtkSmartPointer<vtkColorTransferFunction> Color =  vtkSmartPointer<vtkColorTransferFunction>::New();
	Color->AddRGBPoint(30, 0.0, 0.0, 1.0);
	Color->AddRGBPoint(60, 0.0, 1.0, 1.0);
	Color->AddRGBPoint(90, 0.0, 1.0, 0.0);
	Color->AddRGBPoint(120, 1.0, 1.0, 0.0);
	Color->AddRGBPoint(150, 1.0, 0.0, 0.0);

	vtkSmartPointer<vtkDataSetMapper> StreamSurface_Mapper1 = vtkSmartPointer<vtkDataSetMapper>::New();
	StreamSurface_Mapper1->SetLookupTable(Color);	
	StreamSurface_Mapper1->SetInputConnection(scalarSurface1->GetOutputPort());

	vtkSmartPointer<vtkDataSetMapper> StreamSurface_Mapper2 = vtkSmartPointer<vtkDataSetMapper>::New();
	StreamSurface_Mapper2->SetLookupTable(Color);	
	StreamSurface_Mapper2->SetInputConnection(scalarSurface2->GetOutputPort());
	
	vtkSmartPointer<vtkActor> StreamSurface_Actor1 = vtkSmartPointer<vtkActor>::New();
	StreamSurface_Actor1->SetMapper(StreamSurface_Mapper1);
	StreamSurface_Actor1->GetProperty()->SetOpacity(0.6);

	vtkSmartPointer<vtkActor> StreamSurface_Actor2 = vtkSmartPointer<vtkActor>::New();
	StreamSurface_Actor2->SetMapper(StreamSurface_Mapper2);
	StreamSurface_Actor2->GetProperty()->SetOpacity(0.6);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(Delta_Wing_Geometry_Actor);
	renderer->AddActor(StreamSurface_Actor1);
	renderer->AddActor(StreamSurface_Actor2);

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(400,300);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderWindowInteractor->Initialize();
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}