#include <iostream>
#include <string>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkPNGReader.h>
#include <vtkButtonWidget.h>
#include <vtkTexturedButtonRepresentation.h>
#include <vtkTexturedButtonRepresentation2D.h>
#include <vtkColorTransferFunction.h>
#include <vtkCommand.h>
#include <vtkContourFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetReader.h>
#include <vtkDataSetWriter.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkScalarBarActor.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkTextProperty.h>
#include <vtkWidgetEvent.h>
#include <vtkWidgetEventTranslator.h>

#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

vtkSmartPointer<vtkDataSetMapper> mapper;
double gmin, gmax;
int status = 0;

void setCTF(vtkSmartPointer<vtkColorTransferFunction> ctf) 
{
	ctf->RemoveAllPoints();
	if(status == 0)
	{
		ctf->AddHSVPoint(gmax, 240.0/360, 0.0, 1.0);
		ctf->AddHSVPoint(gmin, 240.0/360, 1.0, 1.0);
	}
	else if(status == 1)
	{
		ctf->AddHSVPoint(gmax, 0.0, 0.94, 0.85);
		ctf->AddHSVPoint(0.5*(gmax+gmin), 0.0, 0.0, 0.85);
		ctf->AddHSVPoint(gmin, 231.0/360, 0.94, 0.85);
	}
	else if(status == 2)
	{
		ctf->AddHSVPoint(gmin, 60.0/360, 1.0, 0.75);
		ctf->AddHSVPoint((1.-1./6)*gmin + 1./6*gmax, 50.0/360, 0.96, 0.69);
		ctf->AddHSVPoint((1.-2./6)*gmin + 2./6*gmax, 42.0/360, 0.92, 0.63);
		ctf->AddHSVPoint((1.-3./6)*gmin + 3./6*gmax, 37.5/360, 0.88, 0.57);
		ctf->AddHSVPoint((1.-4./6)*gmin + 4./6*gmax, 28.5/360, 0.76, 0.44);
		ctf->AddHSVPoint((1.-5./6)*gmin + 5./6*gmax, 19.5/360, 0.85, 0.32);
		ctf->AddHSVPoint(gmax, 3.0/360, 0.98, 0.21);
	}
	else if(status == 3)
	{
		ctf->AddHSVPoint(gmin, 60.0/360, 1.0, 0.75, 0.0, 1.0);
		ctf->AddHSVPoint((1.-1./6)*gmin + 1./6*gmax, 50.0/360, 0.96, 0.69, 0.0, 1.0);
		ctf->AddHSVPoint((1.-2./6)*gmin + 2./6*gmax, 42.0/360, 0.92, 0.63, 0.0, 1.0);
		ctf->AddHSVPoint((1.-3./6)*gmin + 3./6*gmax, 37.5/360, 0.88, 0.57, 0.0, 1.0);
		ctf->AddHSVPoint((1.-4./6)*gmin + 4./6*gmax, 28.5/360, 0.76, 0.44, 0.0, 1.0);
		ctf->AddHSVPoint((1.-5./6)*gmin + 5./6*gmax, 19.5/360, 0.85, 0.32, 0.0, 1.0);
		ctf->AddHSVPoint(gmax, 3.0/360, 0.98, 0.21, 0.0, 1.0);
	}
}

void update() {
	vtkSmartPointer<vtkColorTransferFunction> ctf(dynamic_cast<vtkColorTransferFunction*>(mapper->GetLookupTable()));	
	setCTF(ctf);
}

class vtkButtonCallback : public vtkCommand
{
public:
  static vtkButtonCallback *New()
    { return new vtkButtonCallback; }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
      vtkButtonWidget *buttonWidget =
        reinterpret_cast<vtkButtonWidget*>(caller);
      vtkTexturedButtonRepresentation *rep =
        reinterpret_cast<vtkTexturedButtonRepresentation*>
        (buttonWidget->GetRepresentation());
      status = rep->GetState();
	  update();
      cout << "State: " << status << "\n";
    }
  vtkButtonCallback() {}
};


int main(int argc, char* argv[])
{	
	int w = 1024;
	int h = 768;

	VTK_CREATE(vtkPNGReader, image1);
	VTK_CREATE(vtkPNGReader, image2);
	VTK_CREATE(vtkPNGReader, image3);
	VTK_CREATE(vtkPNGReader, image4);
	VTK_CREATE(vtkDataSetReader, reader);
	
	if(argc == 6) 
	{	
		reader->SetFileName(argv[1]); //vtk file
		image1->SetFileName(argv[2]); //button image1
		image2->SetFileName(argv[3]); //button image2
		image3->SetFileName(argv[4]); //button image3
		image4->SetFileName(argv[5]); //button image4
	}
	else 
		exit(0);

	image1->Update();	
	image2->Update();
	image3->Update();
	image4->Update();
	reader->Update();

	double bds[6];
	bds[0] = 25; bds[1] = 65; bds[2] = 50; bds[3] = 200, bds[4] = 0.0; bds[5] = 0.0;
	VTK_CREATE(vtkButtonCallback, myCallback);
	VTK_CREATE(vtkTexturedButtonRepresentation2D, rep);
	rep->SetNumberOfStates(4);
	rep->SetButtonTexture(0,image1->GetOutput());
	rep->SetButtonTexture(1,image2->GetOutput());
	rep->SetButtonTexture(2,image3->GetOutput());
	rep->SetButtonTexture(3,image4->GetOutput());
	rep->SetPlaceFactor(1);
	rep->PlaceWidget(bds);
	
	double range[2];
	vtkDataArray* array = reader->GetOutput()->GetPointData()->GetScalars();
	array->GetRange(range, 0);
	gmin = range[0];
	gmax = range[1];
	
	vtkFloatArray  *tmp_float_array  = dynamic_cast<vtkFloatArray*>(array);
	vtkDoubleArray *tmp_double_array = dynamic_cast<vtkDoubleArray*>(array);
	bool is_float = (tmp_float_array != NULL || tmp_double_array != NULL);
	tmp_float_array = NULL;
	tmp_double_array = NULL;
	array = NULL;
	
	VTK_CREATE(vtkColorTransferFunction, ctf);
	setCTF(ctf);
	
	VTK_CREATE(vtkScalarBarActor, scalar_bar);
	scalar_bar->SetLookupTable(ctf);
	scalar_bar->SetPosition(0.85, 0.5);
	scalar_bar->SetMaximumWidthInPixels(80);
	scalar_bar->SetMaximumHeightInPixels(400);
	scalar_bar->SetNumberOfLabels(4);
	scalar_bar->SetLabelFormat(is_float ? "%0.2f" : "%0.0f");
	scalar_bar->GetTitleTextProperty()->SetColor(1, 1, 1);
	scalar_bar->GetTitleTextProperty()->ShadowOff();
	scalar_bar->GetTitleTextProperty()->SetFontSize(22);
	scalar_bar->GetTitleTextProperty()->BoldOn();
	scalar_bar->GetLabelTextProperty()->SetColor(1, 1, 1);
	scalar_bar->GetLabelTextProperty()->BoldOff();
	scalar_bar->GetLabelTextProperty()->ShadowOff();
	
	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(reader->GetOutputPort());
	mapper->SetLookupTable(ctf);

	VTK_CREATE(vtkActor, actor);
	actor->SetMapper(mapper);

	VTK_CREATE(vtkRenderer, renderer);
	renderer->AddActor(actor);
	renderer->AddActor(scalar_bar);
	renderer->SetBackground(0, 0, 0);
	renderer->ResetCamera();
	renderer->LightFollowCameraOn();

	VTK_CREATE(vtkRenderWindow, window);
	window->AddRenderer(renderer);
	window->SetSize(w, h);

	VTK_CREATE(vtkRenderWindowInteractor, interactor);
	interactor->SetRenderWindow(window);
	window->Render();
	
	VTK_CREATE(vtkButtonWidget, buttonWidget);
	buttonWidget->SetInteractor(interactor);
	buttonWidget->SetRepresentation(rep);
	buttonWidget->AddObserver(vtkCommand::StateChangedEvent,myCallback);
	buttonWidget->EnabledOn();
	
	interactor->Initialize();
	interactor->Start();

	return 0;
}






