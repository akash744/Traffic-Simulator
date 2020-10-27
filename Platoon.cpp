#include "Platoon.h"

Platoon::Platoon(string init)
{
    // the string 'init' looks like:
    // a,b ; c,d ; ... where, a and c are the vehicle id, b and d are positions.
    stringstream ss(init);
	string segment;
	while(getline(ss, segment, ';'))
	{
		size_t found = segment.find(",");
		if (found!=std::string::npos){
			string id = segment.substr(0, found);
			string pos = segment.substr(found+1, segment.size()-1);

			// conversion to integer
			stringstream s1(id);
			int id_value;
			s1 >> id_value;
			stringstream s2(pos);
			int pos_value;
			s2 >> pos_value;
			Car* car = new Car(id_value, pos_value);

            if (head == NULL)
            {
                head = car;
                tail = car;
            }
            else
            {
                tail->set_next(car);
                car->set_prev(tail);
                tail = car;
                car->set_next(NULL);
            }
		}
	}
}

Car* Platoon::get_tail()
{
	return tail;
}

Car* Platoon::get_head()
{
	return head;
}

void Platoon::prepend(Car *c)
{
  c->set_next(head);
  head=c;
}

void Platoon::append(Car *c)
{
  if(head == NULL){
    head=c;
    return;
  }
  else{
    Car *t;
    t=head;
    while(t->get_next() != 0){
      t = t->get_next();
    }
    t->set_next(c);
  }
}

void Platoon::remove(Car *c)
{
  Car* head_reference = this->get_head();
  Car* tail_reference = this->get_tail();

  if(head_reference == c){
    c->get_next()->set_prev(NULL);
    head = c->get_next();
  }
  else if((c->get_next() != NULL) && (c->get_prev() != NULL)){
    (c->get_prev())->set_next(c->get_next());
    (c->get_next())->set_prev(c->get_prev());
  }
  else if(tail_reference == c){
    (c->get_prev())->set_next(NULL);
		tail = c->get_prev();
		c->set_prev(NULL);
  }
  

}