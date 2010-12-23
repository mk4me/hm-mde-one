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
  // opór
  float frictionMoment = pushed ? clickFriction : airFriction;

  // prêdkoœæ bez uwzglêdnienia oporu
  angularVelocity = angularVelocity + (forcedMoment / momentOfInteria) * deltaTime;
  // delta wynikaj¹ca z oporu (przeciwna do prêdkoœci)
  float frictionVelocity = -1.0f * utils::sign(angularVelocity) * ( frictionMoment / momentOfInteria ) * deltaTime;

  if ( fabs(angularVelocity) < fabs(frictionVelocity) ) {
    // cia³o jest w spoczynku (nie uda³o siê pokonaæ tarcia)
    angularVelocity = 0.0;
  } else {
    // odejmujemy wartoœæ wynikaj¹c¹ z oporu
    angularVelocity += frictionVelocity;
  }

  // delta k¹towa
  delta = 0.5f * angularVelocity * deltaTime;
  // zerujemy wymuszony moment obrotowy
  forcedMoment = 0.0f;

  // nowa wartoœæ
  float prevValue = value;
  value += delta;

  // czy dotarliœmy do granicy?
  if ( value < minValue || value > maxValue ) {
    // zerujemy fizykê, bo jesteœmy na koñcu
    value = utils::clamp(value, minValue, maxValue);
    delta = value - prevValue;
    angularVelocity = 0.0f;
  }

  // aktualizacja
  notify();
}