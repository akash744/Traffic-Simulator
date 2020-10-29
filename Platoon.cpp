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

// Inserts to Front of Linked List
void Platoon::prepend(Car *c)
{
  if(head == NULL){
    head = c;
    tail = c;
  }
  else{
    head->set_prev(c);
    c->set_next(head);
    head = c;
  }
}

// Inserts to End of Linked List
void Platoon::append(Car *c)
{
  if(head == NULL){
    head = c;
    tail = c;
  }
  else if(head != NULL){
    tail->set_next(c);
    c->set_prev(tail);
    tail = c;
  }
}

// Removes to First Node of Linked List
void Platoon::remove_first(Car *c)
{
  if(head->get_next() == NULL){
    
    head = NULL;
    tail = NULL;
  }
  else{
    head = head->get_next();
    
    head->set_prev(NULL);
  }
}

// Removes to Last Node of Linked List
void Platoon::remove_last(Car *c)
{
  if(head->get_next() == NULL){
    
    head = NULL;
    tail = NULL;
  }
  else if(head != NULL){
    tail = tail->get_prev();
    
    tail->set_next(NULL);
  }
}

//Removes Car whether it be at front, end or middle of Linked List
void Platoon::remove(Car *c)
{
  if(c->get_prev() == NULL){
    remove_first(c);
  }
  else if(c->get_next() == NULL){
    remove_last(c);
  }
  else if(head != NULL){
    Car* traversal_position = head;
    while(traversal_position->get_position() != c->get_position()){
      traversal_position = traversal_position->get_next();
    }
    traversal_position->get_prev()->set_next(traversal_position->get_next());
    traversal_position->get_next()->set_prev(traversal_position->get_prev());
    
  }
}

//Inserts Car whether it be at front, end or middle of Linked List
void Platoon::insert(Car *c)
{
  
  if (head == NULL)  //If lane is empty when inserting.
	{
		head = c;
		tail = c;
		c->set_prev(NULL);
		c->set_next(NULL);
	}
  else{             //If lane is not empty when inserting.
    if(c->get_position() < head->get_position()){
      prepend(c);
    }
    else if(c->get_position() > tail->get_position()){
      append(c);
    }
    else if(head != NULL){

      Car* traversal_position = head;

      while(traversal_position->get_position() < c->get_position()){
        traversal_position = traversal_position->get_next();
      }

      c->set_next(traversal_position);
			c->set_prev(traversal_position->get_prev());
			traversal_position->get_prev()->set_next(c);
			traversal_position->set_prev(c);
      
    }
  }
}