#include "VideoPCH.h"
#undef M_PI
#define _USE_MATH_DEFINES
#include <math.h>
#include <utils/Utils.h>
#include "PotentiometerModel.h"

PotentiometerModel::PotentiometerModel()
{
  utils::zero<PotentiometerModelData>(*this);
}

PotentiometerModel::PotentiometerModel( const PotentiometerModel& src ) :
  PotentiometerModelData(src)
{
}

void PotentiometerModel::update( float deltaTime )
{
  // op�r
  float frictionMoment = pushed ? clickFriction : airFriction;

  // pr�dko�� bez uwzgl�dnienia oporu
  angularVelocity = angularVelocity + (forcedMoment / momentOfInteria) * deltaTime;
  // delta wynikaj�ca z oporu (przeciwna do pr�dko�ci)
  float frictionVelocity = -1.0f * utils::sign(angularVelocity) * ( frictionMoment / momentOfInteria ) * deltaTime;

  if ( fabs(angularVelocity) < fabs(frictionVelocity) ) {
    // cia�o jest w spoczynku (nie uda�o si� pokona� tarcia)
    angularVelocity = 0.0;
  } else {
    // odejmujemy warto�� wynikaj�c� z oporu
    angularVelocity += frictionVelocity;
  }

  // delta k�towa
  delta = 0.5f * angularVelocity * deltaTime;
  // zerujemy wymuszony moment obrotowy
  forcedMoment = 0.0f;

  // nowa warto��
  float prevValue = value;
  value += delta;

  // czy dotarli�my do granicy?
  if ( value < minValue || value > maxValue ) {
    // zerujemy fizyk�, bo jeste�my na ko�cu
    value = utils::clamp(value, minValue, maxValue);
    delta = value - prevValue;
    angularVelocity = 0.0f;
  }

  // aktualizacja
  notify();
}