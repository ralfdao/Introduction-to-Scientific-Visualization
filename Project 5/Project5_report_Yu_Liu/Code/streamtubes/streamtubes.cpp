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
#include <vtkPointSource.h>
#include <vtkStreamLine.h>
#include <vtkTubeFilter.h>
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

	vtkSmartPointer<vtkPointSource> seeds1 = vtkSmartPointer<vtkPointSource>::New();
	seeds1->SetRadius(10);
	seeds1->SetCenter(45, 25, 0);
	seeds1->SetNumberOfPoints(20);

	vtkSmartPointer<vtkPointSource> seeds2 = vtkSmartPointer<vtkPointSource>::New();
	seeds2->SetRadius(10);
	seeds2->SetCenter(45, -25, 0);
	seeds2->SetNumberOfPoints(20);

	vtkSmartPointer<vtkRungeKutta4> integ = vtkSmartPointer<vtkRungeKutta4>::New();
	vtkSmartPointer<vtkStreamLine> streamer1 = vtkSmartPointer<vtkStreamLine>::New();
	vtkSmartPointer<vtkStreamLine> streamer2 = vtkSmartPointer<vtkStreamLine>::New();

	streamer1->SetInputConnection(VectorReader->GetOutputPort());
	streamer1->SetSource(seeds1->GetOutput());
	streamer1->SetMaximumPropagationTime(50);
	streamer1->SetStepLength(0.01);
	streamer1->SetIntegrationStepLength(0.01);
	streamer1->SetIntegrationDirectionToIntegrateBothDirections();
	streamer1->SetIntegrator(integ);
	streamer1->VorticityOn();
	streamer1->SpeedScalarsOn();

	vtkSmartPointer<vtkTubeFilter>streamTube1 = vtkSmartPointer<vtkTubeFilter>::New();
	streamTube1->SetInputConnection(streamer1->GetOutputPort());
	streamTube1->SetNumberOfSides(12);
	streamTube1->SetVaryRadiusToVaryRadiusByScalar();
	
	streamer2->SetInputConnection(VectorReader->GetOutputPort());
	streamer2->SetSource(seeds2->GetOutput());
	streamer2->SetMaximumPropagationTime(50);
	streamer2->SetStepLength(0.01);
	streamer2->SetIntegrationStepLength(0.01);
	streamer2->SetIntegrationDirectionToIntegrateBothDirections();
	streamer2->SetIntegrator(integ);
	streamer2->VorticityOn();
	streamer2->SpeedScalarsOn();

	vtkSmartPointer<vtkTubeFilter>streamTube2 = vtkSmartPointer<vtkTubeFilter>::New();
	streamTube2->SetInputConnection(streamer2->GetOutputPort());
	streamTube2->SetNumberOfSides(12);
	streamTube2->SetVaryRadiusToVaryRadiusByScalar();

	vtkSmartPointer<vtkColorTransferFunction> Color =  vtkSmartPointer<vtkColorTransferFunction>::New();
	Color->AddRGBPoint(30, 0.0, 0.0, 1.0);
	Color->AddRGBPoint(60, 0.0, 1.0, 1.0);
	Color->AddRGBPoint(90, 0.0, 1.0, 0.0);
	Color->AddRGBPoint(120, 1.0, 1.0, 0.0);
	Color->AddRGBPoint(150, 1.0, 0.0, 0.0);

	vtkSmartPointer<vtkDataSetMapper> StreamTube_Mapper1 = vtkSmartPointer<vtkDataSetMapper>::New();
	vtkSmartPointer<vtkDataSetMapper> StreamTube_Mapper2 = vtkSmartPointer<vtkDataSetMapper>::New();
	StreamTube_Mapper1->SetLookupTable(Color);	
	StreamTube_Mapper1->SetInputConnection(streamTube1->GetOutputPort());
	StreamTube_Mapper2->SetLookupTable(Color);
	StreamTube_Mapper2->SetInputConnection(streamTube2->GetOutputPort());
	
	vtkSmartPointer<vtkActor> StreamTube_Actor1 = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkActor> StreamTube_Actor2 = vtkSmartPointer<vtkActor>::New();
	StreamTube_Actor1->SetMapper(StreamTube_Mapper1);
	StreamTube_Actor1->GetProperty()->BackfaceCullingOn();
	StreamTube_Actor2->SetMapper(StreamTube_Mapper2);
	StreamTube_Actor2->GetProperty()->BackfaceCullingOn();

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(Delta_Wing_Geometry_Actor);
	renderer->AddActor(StreamTube_Actor1);
	renderer->AddActor(StreamTube_Actor2);

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