#ifndef HEADER_GUARD_KINEMATICMODEL__DEGREEOFFREEDOM_H__
#define HEADER_GUARD_KINEMATICMODEL__DEGREEOFFREEDOM_H__

#include <vector>

namespace kinematic
{
     /*! 
     * \brief Struktura pełni funkcje przestrzeni nazw dla 
              wyliczenia Order i metod z tym związanych
     */
    struct Axis {
        /*!
         * określa kolejność rotacji
         */
        enum Order {
            UnknownAxisOrder,
            XYZ,
            XZY,
            YXZ,
            YZX,
            ZXY,
            ZYX,
        };
        
        /// \brief  Zwraca kolejność rotacji na podstawie nazwy. 
        /// \param  axis Napis określający kolejność rotacji. 
        /// \return Kolejność rotacji. 
        static Order getAxisOrder(const std::string& axis);

        /// \brief  Zwraca nazwę określająca kolejność rotacji. 
        /// \param  order kolejność rotacji 
        /// \return The axis order name. 
        static std::string getAxisOrderName(Order order);
    };

    /// \brief  Struktura przechowuje pojedynczy stopien swobody dla kości.
    struct DegreeOfFreedom
    {
        /// \brief Wyliczenie zawiera wszystkie mozlwie kanały animacji. 
        /// T oznacza translacje, R rotacje, L przesunięcie zgodne z kierunkiem kości
        enum Channel
        {
            Unknown,
            TX,
            TY,
            TZ,
            RX,
            RY,
            RZ,
            L
        };

        Channel channel; //<! kanał, dla którego definiowany jest stopien swobody
        double minLimit; //<! minimalna wartość dla kanału (domyślnie -nieskonczoność)
        double maxLimit; //<! maksymalna wartość dla kanału (domyślnie nieskonczoność)

        // konstruktor
        DegreeOfFreedom() :
            channel(Unknown),
                minLimit(-std::numeric_limits<double>::infinity()),
                maxLimit(std::numeric_limits<double>::infinity())
            {
            }

        /// \brief  Zwraca indeks (w tablicy wartości kanałów dla pojedynczej klatki kości)
        /// \param  Kanał dla którego powinnien być zwrócony indeks 
        /// \param  dofs lista ze stopniami swobody (na jej podstawie ustalany jest indeks)
        /// \return Indeks kanału lub -1 jeśli kanału nie znaleziono
        static int getChannelIndex(DegreeOfFreedom::Channel channel, const std::vector<DegreeOfFreedom>& dofs);
        /// \brief  Zwraca kanał na podstawie nazwy.
        /// 		jeśli kanał nie zostanie znaleziony to rzucany jest wyjątek 
        /// \param  channel nazwa kanału. 
        /// \return otrzymany kanał. 
        static Channel getChannel(const std::string& channel);
        /// \brief  Zwraca nazwę kanału na postawie wyliczenia. 
        /// \param  channel kanał dla którego będzie zwrócona nazwa. 
        /// \param  uppercase określa czy zwracana nazwa ma mieć duże, czy male litery. 
        /// \return Nazwa kanału. 
        static std::string getChannelName(DegreeOfFreedom::Channel channel, bool uppercase = false);
    };
}

#endif
