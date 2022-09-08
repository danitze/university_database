# university database :book:

The application was created in order to illustrate the work of a relational database (like SQL).

The programme consists of two tables: **universities** and **faculties**, with a one-to-many relationship (cascade strategy on both updating and deleting).

## Tables structure
**University**:mortar_board:<br />
[<br />
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ID: Integer (Primary Key)<br />
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Name: String<br />
]<br />

**Faculty**:school:<br />
[<br />
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ID: Integer (Primary Key)<br />
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;University ID: Integer (Foreign Key)<br />
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Name: String<br />
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Address: String<br />
]<br />

## Functionality
University database accepts the following functionality:
* Insert a record
* Delete a record
* Update a record
* Get a record
* Get a count of records on each table
* Get the amount of faculties at a certain university
* Show full information about a table
