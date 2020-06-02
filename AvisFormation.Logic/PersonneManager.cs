using AvisFormation.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AvisFormation.Logic
{
    public class PersonneManager
    {
        public void InsertNom(string userId, string nom)
        {
            using (var context = new AvisEntities())
            {
              var personEntity = context.Personne.FirstOrDefault(p => p.UserId == userId);
              if(personEntity == null)
                {
                    var p = new Personne();
                    p.Nom = nom;
                    p.UserId = userId;
                    context.Personne.Add(p);
                    context.SaveChanges();
                }

            }
        }

        public string GetNomFromUserId(string userId)
        {
            using (var context = new AvisEntities())
            {
                var personEntity = context.Personne.FirstOrDefault(p => p.UserId == userId);
                if (personEntity == null)
                {
                    return "Anonyme";
                }
                return personEntity.Nom;

            }
        }
    }
}
