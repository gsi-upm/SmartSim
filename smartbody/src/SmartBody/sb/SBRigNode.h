#ifndef _SBRIGNODE_
#define _SBRIGNODE_


namespace SmartBody {

class SBRigNode
{
	public:
		SBRigNode();
		~SBRigNode();
};

class SBSetDrivenKeyNode : public SBRigNode
{
	public:
		SBSetDrivenKeyNode();
		~SBSetDrivenKeyNode();

		void setRange(double range);
		double getValue(double value);
			
	protected:
		double _range;
};

class SBSetDrivenKeySequenceNode : public SBRigNode
{
	public:
		SBSetDrivenKeySequenceNode();
		~SBSetDrivenKeySequenceNode();

		void setRange(double range, int numValues);
		double getValue(double value, int index);
			
	protected:
		double _range;
		int _numValues;
};

}




#endif