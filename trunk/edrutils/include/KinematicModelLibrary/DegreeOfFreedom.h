#ifndef HEADER_GUARD_KINEMATICMODEL__DEGREEOFFREEDOM_H__
#define HEADER_GUARD_KINEMATICMODEL__DEGREEOFFREEDOM_H__

namespace hmAnimation
{
     /*! 
     * \brief Struktura pelni funkcje przestrzeni nazw dla 
              wyliczenia Order i metod z tym zwiazanych
     */
    struct Axis {
        /*!
         * Okresla kolejnosc rotacji
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
        
        /// \brief  Zwraca kolejnosc rotacji na podstawie nazwy. 
        /// \param  axis Napis okreslajacy kolejnosc rotacji. 
        /// \return Kolejnosc rotacji. 
        static Order getAxisOrder(const std::string& axis);

        /// \brief  Zwracja nazwe okreslajaca kolejnosc rotacji. 
        /// \param  order kolejnosc rotacji 
        /// \return The axis order name. 
        static std::string getAxisOrderName(Order order);
    };

    /// \brief  Struktura przechowuje pojedynczy stopien swobody dla kosci.
    struct DegreeOfFreedom
    {
        /// \brief Wyliczenie zawiera wszystkie mozlwie kanaly animacji. 
        /// T oznacza translacje, R rotacje, L przesuniecie zgodne z kierunkiem kosci
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

        Channel channel; //<! kanal, dla ktorego definiowany jest stopien swobody
        double minLimit; //<! minimalna wartosc dla kanalu (domyslnie -nieskonczonosc)
        double maxLimit; //<! maksymalna wartosc dla kanalu (domyslnie nieskonczonosc)

        // konstruktor
        DegreeOfFreedom() :
            channel(Unknown),
                minLimit(-std::numeric_limits<double>::infinity()),
                maxLimit(std::numeric_limits<double>::infinity())
            {
            }

        /// \brief  Zwraca indeks (w tablicy wartosci kanalow dla pojedynczej klatki kosci)
        /// \param  Kana³ dla ktorego powinnien byc zwrocony indeks 
        /// \param  dofs lista ze stopniami swobody (na jej podstawie ustalany jest indeks)
        /// \return Indeks kana³u lub -1 jesli kanalu nie znaleziono
        static int getChannelIndex(DegreeOfFreedom::Channel channel, const std::vector<DegreeOfFreedom>& dofs);
        /// \brief  Zwraca kanal na podstawie nazwy.
        /// 		Jesli kanal nie zostanie znaleziony to rzucany jest wyjatek 
        /// \param  channel nazwa kanalu. 
        /// \return otrzymany kanal. 
        static Channel getChannel(const std::string& channel);
        /// \brief  Zwraca nazwe kanalu na postawie wyliczenia. 
        /// \param  channel Kanal dla ktorego bedzie zwrocona nazwa. 
        /// \param  uppercase Okresla czy zwracana nazwa ma miec duze, czy male litery. 
        /// \return Nazwa kanalu. 
        static std::string getChannelName(DegreeOfFreedom::Channel channel, bool uppercase = false);
    };
}

#endif